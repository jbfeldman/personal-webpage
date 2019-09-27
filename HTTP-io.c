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


#define PUBLIC_FOLDER "public"
#define INDEX_FILE "public/index.html"




void serve_static_content(char *url, int sockfd);
void serve_dynamic_content(char* buffer, int n, int sockfd);
void construct_filepath(char* fpath, int fpath_len, char* folder, char* url);

void process_GET_request(struct HTTP_request* header, int sockfd){

    //if requesting index file
    if strcmp(header->url, "/"){
        write_file(INDEX_file, sockfd);
    }
    /* else if ...chains of else if statements to set up specialized API endpoints */
    else{
        serve_static_content(header->url, sockfd);
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
void serve_static_content(char *url, int sockfd){

    int fpath_len = MAX_FNAME_SIZE + strlen(PUBLIC_FOLDER) + 1;
    char filepath[fpath_len + 1];
    construct_filepath(filepath, fpath_len, PUBLIC_FOLDER, url);

    if( access( filepath, F_OK ) == -1 ){
        send_404_error(sockfd);
        return;
    }

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
    bzero(filepath, fpath_len + 1);
    strncpy(fpath, PUBLIC_FOLDER, strlen(PUBLIC_FOLDER));

    //should always leave final byte of buffer as null byte
    strncpy(&(fpath[strlen(PUBLIC_FOLDER)]), header->url, fpath_len - strlen(PUBLIC_FOLDER));


}