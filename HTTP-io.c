#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <strings.h>

#include "HTTP-io.h"
#define MAX_FNAME_SIZE 100
#define MAX_DIGITS 21


#define PUBLIC_FOLDER "public"
#define INDEX_FILE "public/index.html"
#define SERVER_NAME "Jonah's custom server"




void serve_static_content(char *url, int sockfd, struct HTTP_response response);
void serve_dynamic_content(char* buffer, int n, int sockfd);
void construct_filepath(char* fpath, int fpath_len, char* folder, char* url);
void write_response_header(int sockfd, struct HTTP_response header);
unsigned long get_file_length(char* fname);

void process_GET_request(struct HTTP_request* header, int sockfd){

    //TODO: maybe function this out to "initialize response"
    char con_len_buffer[MAX_DIGITS];
    bzero(con_len_buffer, MAX_DIGITS);

     struct HTTP_response response = 
    {
        .status = "HTTP/1.1 200 OK",
        .date = "Today", //TODO: Change this to a get_date() funcion
        .server = SERVER_NAME,
        .content_length = con_len_buffer, 
        .content_type = "text/html"
    };

    /*IMPORTANT! For every API endpoint that is not serve_static_content, the field content_length must be set*/
    //if requesting index file
    if (strcmp(header->url, "/") == 0){

        sprintf(response.content_length, "%lu", get_file_length(INDEX_FILE));
        write_response_header(sockfd, response);
        write_file(sockfd, INDEX_FILE);
    }
    /* else if ...chains of else if statements to set up specialized API endpoints */
    else{
        serve_static_content(header->url, sockfd, response);
    }
}

/*
 * Purpose: Writes an HTTP header and html file to the client.  If the file does
 *          not exist or another problem with the HTTP header occurs, the function
 *          instead writes an error code (and possibly error page) back to the client
 * Args:
 *      url: the filepath of the request file
 *      sockfd: the socket file descriptor
 */
void serve_static_content(char *url, int sockfd, struct HTTP_response response){

    int fpath_len = MAX_FNAME_SIZE + strlen(PUBLIC_FOLDER) + 1;
    char filepath[fpath_len + 1];
    construct_filepath(filepath, fpath_len, PUBLIC_FOLDER, url);

    //if requested file does not exist
    if( access( filepath, F_OK ) == -1 ){
        send_404_error(sockfd);
        return;
    }

    sprintf(response.content_length, "%lu", get_file_length(filepath));
   
    write_response_header(sockfd, response);
    write_file(sockfd, filepath);
    fprintf(stderr, "wrote back file %s\n", filepath );
}

/*
 * Purpose: constructs filepath from a url and the name of the folder containing
 *          the public 
 * Args:
 *      fpath: unitialized strings where the final fpath will be placed into
 *      fpath_len: the length of the fpath string
 *      folder: the folder we want to access
 *      url: the url being requested by the client
 *
 */
void construct_filepath(char* fpath, int fpath_len, char* folder, char* url ){
    bzero(fpath, fpath_len + 1);
    strncpy(fpath, folder, strlen(folder));

    //should always leave final byte of buffer as null byte
    strncpy(&(fpath[strlen(folder)]), url, fpath_len - strlen(folder));


}

void send_404_error(int sockfd){
    char* message = "HTTP/1.1 404 Not Found\r\n\r\n";
    write_message(sockfd, message, strlen(message));
}

void send_403_error(int sockfd){
    char* message = "HTTP/1.1 403 Forbidden\r\n\r\n";
    write_message(sockfd, message, strlen(message));
}

void send_401_error(int sockfd){
    char* message = "HTTP/1.1 401 Unauthorized\r\n\r\n";
    write_message(sockfd, message, strlen(message));
}

void send_400_error(int sockfd){
    char* message = "HTTP/1.1 400 Bad Request\r\n\r\n";
    write_message(sockfd, message, strlen(message));
}




/*
 * Purpose: write an HTTP response header to the client
 * Args:
 *      sockfd: the socket file descriptor of the client
 *      header: the HTTP header struct to be sent
 *
 */
void write_response_header(int sockfd, struct HTTP_response header){

    char *buffer = NULL;
    char **buf_ptr = &buffer;


    int n = make_HTTP_response_header(header, buf_ptr);
    fprintf(stderr, "response header is \n%s", *buf_ptr);
    write_message(sockfd, *buf_ptr, n);
    free (*buf_ptr);

}

//returns the file length for a given file
unsigned long get_file_length(char* fname){
    FILE *fp = fopen(fname, "rb");
    unsigned long filelen;
    fseek(fp, 0, SEEK_END);
    filelen = ftell(fp);
    rewind(fp);
    fclose(fp);
    return filelen;
}