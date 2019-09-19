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
#define INDEX_FILE "public/index.html"

int accept_connection(int lSock);
int process_request(int sockfd);
void write_request_to_file(char* buf, int n);


//TODO: function out all of the lines relevant to accepting a connection
    // TODO: open index.html
    // TODO: write line by line


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

    write_request_to_file(buf, n);

   

    write_file(sockfd, INDEX_FILE);
    return DISCONNECT_CODE;
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
