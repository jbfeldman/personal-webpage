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
int accept_connection(lSock);


//TODO: function out all of the lines relevant to accepting a connection
    // TODO: open index.html
    // TODO: write line by line


int main(int arg, char** argv){
    int lSock, maxSock, rv, newSock = -1;

    struct timeval tv;
    tv.tv_sec=1;
    tv.tv_usec=1000;
    fd_set masterFDSet, copyFDSet;
    FD_ZERO(&masterFDSet);
    FD_ZERO(&copyFDSet);
    FD_SET(lSock, &masterFDSet);

    lSock = initialize_listening_sock(DEFAULT_PORT_NO);
    maxSock = lSock;

    while (1){
        FD_ZERO(&copyFDSet);
        memcpy(&copyFDSet, &masterFDSet, sizeof(masterFDSet));
        rv = select (maxSock + 1, &copyFDSet, NULL, NULL, &tv);
   
        if (rv == -1)
            perror("Select");
        else if (rv ==0)
            ;
        else{
            for (int sockfd = 0; sockfd < maxSock + 1; sockfd++){
                //if there is any connection or new data to be read
                if ( FD_ISSET (sockfd, &copyFDSet) ) {
                    if (sockfd == lSock){
                        newSock = accept_connection(lSock);
                        if (newSock > 0){
                            FD_SET(newSock, &masterFDSet);
                            maxSock = (newSock > maxSock) ? newSock: maxSock;
                            //add to sockArray
                        }
                        
                    }
                    else{
                        write_response(sockfd);
                        if (status == DISCONNECT_CODE || 1 /*delete the or 1, rn always disconnects client */){
                            //TODO: write a disconnect function
                            close(sockfd);
                            FD_CLR(sockfd, masterFDSet);
                            
                            
                        }

                    }
                }
            }
        }
    }

    
    //fprintf(stderr, "accepted new socket with sockfd %d\n", cliSock);
    return 0;

}

/* Purpose: intialize the server socket and bind it to a portno
 * Args: Portno, the port number that will be bound to (will almost always be 80)
 * Returns: the socket file descriptor of the listening socket
 */
int initialize_listening_sock(int portno){
    int sockfd;
    
    
    struct sockaddr_in serv_addr;
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

/* Purpose: To accept an incoming connection from a new client
 * Args: listening socket file descriptor
 * Returns: socket file descriptor for new client
 */
int accept_connection(lSock){
    struct sockaddr_in cli_addr;
    socklen_t clilen;
    clilen = sizeof(cli_addr);

    return accept(lSock, (struct sockaddr *) &cli_addr,  &clilen);

}
