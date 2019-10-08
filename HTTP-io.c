#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <strings.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>


#include "HTTP-io.h"
#define MAX_FNAME_SIZE 100
#define MAX_FIELD_SIZE 100

#define PUBLIC_FOLDER "public"
#define INDEX_FILE "public/index.html"
#define SERVER_NAME "Jonah's custom server"



void create_HTTP_date(char* date_buffer);
void serve_static_content(char *url, int sockfd, struct HTTP_response response);
void serve_dynamic_content(char* buffer, int n, int sockfd);
void construct_filepath(char* fpath, int fpath_len, char* folder, char* url);
void write_response_header(int sockfd, struct HTTP_response header);
unsigned long get_file_length(char* fname);
void determine_content_type(char *content_type_buffer, char *filename);
char file_exists(char *filename);


void process_GET_request(struct HTTP_request* header, int sockfd){

    //TODO: this is ugly, probably just going to have to bite the bullet and use malloced strings then deal with the memory issues 
    char con_len_buffer[MAX_FIELD_SIZE];
    bzero(con_len_buffer, MAX_FIELD_SIZE);
    char date_buffer[MAX_FIELD_SIZE];
    bzero(date_buffer, MAX_FIELD_SIZE);
    char content_type_buffer[MAX_FIELD_SIZE];
    bzero(content_type_buffer, MAX_FIELD_SIZE);
    strcpy(content_type_buffer, "text/html");

     struct HTTP_response response = 
    {
        .status = "HTTP/1.1 200 OK",
        .date = date_buffer, 
        .server = SERVER_NAME,
        .content_length = con_len_buffer, 
        .content_type = content_type_buffer
    };

    create_HTTP_date(date_buffer);

    /*IMPORTANT! For every API endpoint that is not serve_static_content, the field content_length must be set*/
    /* IMPORTANT! If returning content that is NOT html, content_type field must be set to the correct type */

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
    if (!file_exists(filepath)){
        send_404_error(sockfd);
        return;
    }

    determine_content_type(response.content_type, filepath);

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
    fprintf(stderr, "wrote back 404 error\n");
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

/*
 * Purpose: creates an HTTP-compliant date of the current moment
 *          and loads it into date_buffer
 * Args: An array of MAX_FIELD_SIZE bytes loaded with null bytes
 */
void create_HTTP_date(char* date_buffer){

    time_t now = time(0);
    struct tm tm = *gmtime(&now);
    strftime(date_buffer, MAX_FIELD_SIZE, "%a, %d %b %Y %H:%M:%S %Z", &tm);

}


/*
 * Purpose: determines the correct MIME content-type from a given filename.
 *          Will fail if filename does not exist or either arg is NULL
 * Args: 
 *     content_type_buffer: the buffer the content-type will be loaded onto. Should
 *                         be alloc'd or stack'd to at least MAX_FIELD_SIZE
 *      filename: the filename we'll be determining content-type of 
 *
 * 
 */
void determine_content_type(char *content_type_buffer, char* filename){

    char *file_extension = strstr(filename, ".");
    if (file_extension == NULL) return;

    bzero(content_type_buffer, MAX_FIELD_SIZE);

    //Gets content_type from file_extension. Still a whoooooole lot more to add
    if (strcmp(file_extension, ".html") == 0){
        strncpy(content_type_buffer, "text/html", MAX_FIELD_SIZE);
        return;
    }
    else if (strcmp(file_extension, ".css") == 0 || strcmp(file_extension, ".css.map") == 0 || strcmp(file_extension, "cscc") == 0){
        strncpy(content_type_buffer, "text/css", MAX_FIELD_SIZE);
        return;
    }
    else if (strcmp(file_extension, ".svg") == 0){
        strncpy(content_type_buffer, "image/svg+xml", MAX_FIELD_SIZE);
        return;
    }
    else if (strcmp(file_extension, ".xml") == 0){
        strncpy(content_type_buffer, "text/xml", MAX_FIELD_SIZE);
        return;
    }

    else{
        strncpy(content_type_buffer, "application/octet-stream", MAX_FIELD_SIZE);
        return; 
    }

}
/* Purpose: determines if a given file exists
 * Args: the filename in question
 * Returns: 0 if file does not exist, 1 if it does
 */

char file_exists(char* filename){

    //If file OR directory by that name does not exist
    struct stat path_stat;
    stat(filename, &path_stat);
    return S_ISREG(path_stat.st_mode);



}