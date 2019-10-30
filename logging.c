#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/socket.h>


#include "logging.h"

#define LOGS_FOLDER "logs"
#define HTTP_LOG "logs/http-log.txt"
#define CONNECTION_LOG "logs/connections-log.txt"
#define ERROR_LOG "logs/error-log.txt"
#define MAX_IP_LENGTH 50
#define MAX_DATE_LENGTH 100

void create_directory(char* dir_name);
int find_client_ip(int sockfd, char* buffer);


void log_error(int sockfd, char* error){
    create_directory(LOGS_FOLDER);
    FILE *fp = fopen(ERROR_LOG, "a");
    if (fp == NULL) return;
    char ip_buffer[MAX_IP_LENGTH];
    char date_buffer(MAX_DATE_LENGTH);
    find_client_ip(sockfd, ip_buffer);
    format_date(date_buffer)
    //TODO: get and format DATE
    fprintf(fp, "Error \"%s\" was encountered when interacting with client %s\n", error, ip_buffer );

}
void log_request(int sockfd, char* request);
void log_response(int sockfd, char* response);
void log_connection(int sockfd);

/* 
 * Finds the IP address of a client from their given socket file descriptor
 * Stores the IP address in the buffer
 * Args: 
 *      sockfd: the file descriptor of the client socket
 *      buffer: the buffer of MAX_IP_LENGTH to store the IP addr string
 * Returns: 0 upon success, 1 if failure (most likely IP address cannot be found)
 */
int find_client_ip(int sockfd, char* buffer){
    bzero(buffer, MAX_IP_LENGTH);
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    int err = getpeername(sockfd, (struct sockaddr *) &addr, &addr_len);
    if (err != 0) {
       return 1;
    }
    strncpy(buffer, inet_ntoa(addr.sin_addr), MAX_IP_LENGTH);
    return 0;

}


/*
 * Purpose: checks if a directory exists. If directory does not exist, creates directory
 * Args: dir_name: string of directory to create
 *
 */
void create_directory(char* dir_name){
    struct stat st = {0};

    if (stat(dir_name, &st) == -1) {
        mkdir(dir_name, 0700);
    }

}

void format_date(char* date_buffer){
    bzero(date_buffer, MAX_DATE_LENGTH);
    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    strftime(date_buffer, MAX_DATE_LENGTH, "%a, %d %b %Y %H:%M:%S %Z", &tm);

}
