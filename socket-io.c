#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <strings.h>

#define FILE_BUFFER_MAX_LEN 10000

void error(const char *msg) {
    perror(msg);
    exit(0);
}

int open_and_bind_socket(int portno) {
    struct sockaddr_in serv_addr;
    int master_socket;

    master_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (master_socket < 0) {
        error("ERROR opening socket");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(master_socket, (struct sockaddr *) &serv_addr,
             sizeof(serv_addr)) < 0){
        error("ERROR on binding");
    }
    listen(master_socket, 5);

    return master_socket;
}

int connect_to_server(char *ip, int portno) {
    struct sockaddr_in serv_addr;

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    serv_addr.sin_addr.s_addr = inet_addr(ip);
    if (connect(sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    return sockfd;
}

int write_message(int csock, char *data, int length) {
    int m = 0;
    int n = 0;

    while (m < length) {
        n = write(csock, &data[n], length - m);
        if (n < 0) {
            fprintf(stderr, "socket closed\n");
            return -1;
        }
        m += n;
    }

    return 0;
}


int write_file(int csock, char *filename) {
    FILE *fp = fopen(filename, "rb");
    char bytes[FILE_BUFFER_MAX_LEN];
    long filelen;
    int to_write, bytes_written = 0;

    fseek(fp, 0, SEEK_END);
    filelen = ftell(fp);
    rewind(fp);

    while (bytes_written < filelen) {
        if (filelen - bytes_written < FILE_BUFFER_MAX_LEN) {
            to_write = filelen - bytes_written;
        } else {
            to_write = FILE_BUFFER_MAX_LEN;
        }
        fread(bytes, 1, to_write, fp);
        if (write_message(csock, bytes, to_write) < 0)
            return -1;
        bytes_written += to_write;
    }

    fclose(fp);
    return 0;
}

