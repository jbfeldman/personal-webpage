#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#define DEFAULT_PORT_NO 80
int initialize_listening_sock(int portno);
void write_response(/*char* fname,*/ int socket);


//TODO: function out all of the lines relevant to accepting a connection
    // TODO: open index.html
    // TODO: write line by line

int main(int arg, char** argv){
    int lSock;
    int cliSock;

    struct sockaddr_in cli_addr;
    socklen_t clilen;
    clilen = sizeof(cli_addr);

    
    lSock = initialize_listening_sock(DEFAULT_PORT_NO);
    cliSock = accept(lSock, (struct sockaddr *) &cli_addr,  &clilen);
    //fprintf(stderr, "accepted new socket with sockfd %d\n", cliSock);
    write_response(cliSock);
    close(cliSock);
    return 0;

}

/* Purpose: intialize the server socket and bind it to a portno
 * Args: Portno, the port number that will be bound to (will almost always be 80)
 * Returns: the socket file descriptor of the listening socket
 */
int initialize_listening_sock(int portno){
    int sockfd;
    
    
    struct sockaddr_in serv_addr;
    int n;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
       perror("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
             perror("ERROR on binding");
    listen(sockfd,5);
    return sockfd;

}

/* Purpose: writes the response to the client, will need major reworking
 * Args: fname of the file to be returned, and socket file descriptor to write to
 */

void write_response(/*char* fname,*/int socket){
    char *response = "This is the sample response string, here ye here ye";
    int n = write(socket, response, strlen(response));
    if (n < 0){
        perror("error writing to socket");
    }
    else{
        fprintf(stderr, "wrote message of %d bytes\n", n);
    }

}
