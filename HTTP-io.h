#include "HTTP-parser.h"
#include "socket-io.h"


/* 
 * Purpose: these functions write error messages to the client
 *          if something went wrong with the request, also 
 *          writing an error file if there is one:
 * Args: the socket file descriptor of the client socket
 *
 */
void send_400_error(int sockfd);
void send_401_error(int sockfd);
void send_403_error(int sockfd);
void send_404_error(int sockfd);

void process_GET_request(struct HTTP_request* header, int sockfd);
// void process_POST_request(struct HTTP_request* header, int sockfd);
// void process_PUT_request(struct HTTP_request* header, int sockfd);
// void process_DELETE_request(struct HTTP_request* header, int sockfd);