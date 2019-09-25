#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include "io.h"
#include "HTTP-parser.h"

#define DEFAULT_PORT_NO 80
#define DISCONNECT_CODE -69
#define DEFAULT_BUFFER_SIZE 10000
#define REQUEST_LOG "logging/request-log.txt"
#define ERROR_LOG "logging/error-log.txt"
#define PUBLIC_FOLDER "public"
#define INDEX_FILE "public/index.html"
#define GET "GET"
#define MAX_FNAME_SIZE 100

int accept_connection(int lSock);
int process_request(int sockfd);
void write_request_to_file(char* buf, int n);
void send_response(struct HTTP_request* header, /*char *params,*/ int sockfd);


//TODO: function out all of the lines relevant to accepting a connection
    // TODO: open index.html
    // TODO: write line by line
//TODO: rename io.h/.c to socket-io.c/.h, and create new files called HTTP-io.c/.h that
//      will handle and abstract away receiving and sending HTTP requests/responses
//TODO: Everytime you use write_file or write_message, make sure to error check
//TODO: probably should create files "APIs.c" and APIs.h


int main(int arg, char** argv){
    int lSock, maxSock, rv, newSock = -1, status = 0;

    struct timeval tv;
    tv.tv_sec=1;
    tv.tv_usec=1000;
    fd_set masterFDSet, copyFDSet;

    lSock = open_and_bind_socket(DEFAULT_PORT_NO);

    FD_ZERO(&masterFDSet);
    FD_ZERO(&copyFDSet);
    FD_SET(lSock, &masterFDSet);

    maxSock = lSock;

    while (1){
        FD_ZERO(&copyFDSet);
        memcpy(&copyFDSet, &masterFDSet, sizeof(masterFDSet));
        rv = select (maxSock + 1, &copyFDSet, NULL, NULL, &tv);
   
        if (rv == -1)
            error("Select");
        else if (rv ==0)
            ;
        else{
            for (int sockfd = 0; sockfd < maxSock + 1; sockfd++){
                //if there is any connection or new data to be read
                if ( FD_ISSET (sockfd, &copyFDSet) ) {
                    if (sockfd == lSock){
                        newSock = accept_connection(lSock);
                        if (newSock > 0){
                            fprintf(stderr, "there's a new connection in town\n" );
                            FD_SET(newSock, &masterFDSet);
                            maxSock = (newSock > maxSock) ? newSock: maxSock;
                            //add to sockArray
                        }
                        
                    }
                    else{
                        fprintf(stderr, "message from existing connection\n" );

                        status = process_request(sockfd);
                        

                        if (status == DISCONNECT_CODE ){
                            //TODO: write a disconnect function
                            fprintf(stderr, "disconnecting client\n" );
                            close(sockfd);
                            FD_CLR(sockfd, &masterFDSet);
                            
                            
                        }

                    }
                }
            }
        }
    }

    
    //fprintf(stderr, "accepted new socket with sockfd %d\n", cliSock);
    return 0;

}


/* Purpose: To accept an incoming connection from a new client
 * Args: listening socket file descriptor
 * Returns: socket file descriptor for new client
 */
int accept_connection(int lSock){
    struct sockaddr_in cli_addr;
    socklen_t clilen;
    clilen = sizeof(cli_addr);

    return accept(lSock, (struct sockaddr *) &cli_addr,  &clilen);

}

/*
 * Reads in a message from the client, processes it, and returns a status code
 * Only argument is the socket file descriptor
 * TODO: Make the buffer size dynamic instead of just 10k static bytes
 * TODO: split this into functions: Read request, determine response, do response
*/
int process_request(int sockfd){
    int n = 0;
    char buf[DEFAULT_BUFFER_SIZE];
    bzero(buf, DEFAULT_BUFFER_SIZE);

    n = read (sockfd, buf, DEFAULT_BUFFER_SIZE);
    fprintf(stderr, "%d bytes read from client\n", n);
    if (n < 0) error("error reading from socket");
    if (n == 0) return DISCONNECT_CODE;
    struct HTTP_request* header = parse_request(buf);
    //TODO: if header == NULL, send back "invalid request" error
    fprintf(stderr, "request type is \"%s\"\n", header->type);
    fprintf(stderr, "request url is \"%s\"\n", header->url);
    fprintf(stderr, "request host is \"%s\"\n", header->host);

    write_request_to_file(buf, n);

    send_response(header, /*char *params,*/ sockfd);
   
    free_HTTP_request(header);
    //write_file(sockfd, INDEX_FILE);
    return DISCONNECT_CODE;
}

/* 
 * Purpose: determines what type of response is appropiate for given request and
 *          sends that response
 *  Args: 
 *       header: the HTTP header struct
 *       params (TODO: not yet implemented): the HTTP paramaters for 
 *       sockfd: socket file descriptor that will be used to write to client
*/
//TODO: write a function for each possible HTTP request type: GET, POST, etc.
//TODO: before sending out a response, should always have "write_header()" function
void send_response(struct HTTP_request* header, /*char *params,*/ int sockfd){
    fprintf(stderr, "in send_resposne\n" );
    if (header == NULL || header->type == NULL || header->url == NULL || header->host == NULL){
        char *response = "improperly formatted request";
        fprintf(stderr, "%s\n", response);
        write_message(sockfd, response, strlen(response) + 1);

    }
    if (strcmp(header->type, GET) == 0){
        //edge case out index file
        if (strcmp(header->url, "/") == 0 ){
            //write_header();
            fprintf(stderr, "wrote index file back\n");
            write_file(sockfd, INDEX_FILE);
            return;
        }

        //TODO: function this out to construct_filepath
        int fpath_len = MAX_FNAME_SIZE + strlen(PUBLIC_FOLDER) + 1;
        char filepath[fpath_len + 1];
        bzero(filepath, fpath_len + 1);
        strncpy(filepath, PUBLIC_FOLDER, strlen(PUBLIC_FOLDER));

        //should always leave final byte of buffer as null byte
        strncpy(&(filepath[strlen(PUBLIC_FOLDER)]), header->url, fpath_len - strlen(PUBLIC_FOLDER));

        fprintf(stderr, "filepath is %s\n", filepath);

        if( access( filepath, F_OK ) == -1 ){

            //TODO: replace this error message with a proper error response
            fprintf(stderr, "wrote back 404 error\n" );
            write_message(sockfd, "404", 4);
            return;
        } 
        write_file(sockfd, filepath);
        fprintf(stderr, "wrote back file %s\n", filepath );
        return;
        

        //do stuff
    }
    //TODO: else if POST, PUT, DELETE
    
}
/*
 * Purpose: Writes the request (usually HTTP) from server to a file
 * Args: The buffer containing the message and its length
 * No Return value
 * TODO: instead of erroring, maybe just have it return a disconnect code
 */
void write_request_to_file(char* buf, int n)
{
    FILE *fp = fopen(REQUEST_LOG, "a");
    if (fp == NULL) error("error opening REQUEST_LOG");
    if (fwrite(buf, n, 1, fp) != 1) error("error writing to REQUEST_LOG");
    fclose(fp);


}
