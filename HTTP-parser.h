/*
 * HTTP-parser
 * defines structs HTTP-request and HTTP-response and functions that can convert them to a from strings
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <strings.h>

// struct HTTP_request;
// struct HTTP_response;

struct HTTP_request{

    char *type; //GET, POST, PULL, etc
    char *url; //relative URL path
    char *host;
    /*char* conncetion;
    char *cache_control;
    char *user_agent; 
    char* data; //for the body if we migrate to post requests*/

};
struct HTTP_response{
    char *status;
    char *date;
    char *server;
    char *content_length; 
    char *content_type;
};



/*
 * Purpose: converts an HTTP request string into a hTTP_request struct
 * Args: Buffer containing the request, and the length of the request
 * Returns: a struct containing the HTTP-request information
 */
struct HTTP_request* parse_request(char* buffer);

/*
 * Purpose: creates an HTTP response string from an HTTP_response struct
 * Args: 
 *      HTTP_response: a struct containing the relevant Response information
 *      buffer: a pointer to the C-String that will contain the ALLOCATED HTTP_response string
 *Returns: the length of the HTTP response string
 */
int make_HTTP_response_header(struct HTTP_response response_header, char** buffer);

/* 
 * frees HTTP_request header struct pointer
 *
 */
void free_HTTP_request(struct HTTP_request* request);
