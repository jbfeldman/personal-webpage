#include "HTTP-parser.h"

void get_first_line (struct HTTP_request* header, char *buffer);
void get_field(char **field, char *field_string, char *buffer);

struct HTTP_request* parse_request(char* buffer){
    struct HTTP_request* header = malloc(sizeof(struct HTTP_request));

    get_first_line(header, buffer);
    get_field(&(header->host), "Host: ", buffer);
    // can add more fields here

    return header;
}
int make_HTTP_response_header(struct HTTP_response response_header, char** buffer){
    return 0;
}
void free_HTTP_request(struct HTTP_request* request){
    ;
}



/*
 * Purpose: extracts the 'type' and 'url' fields from the first line of the 
            HTTP header string and copies them into the struct
 * Args: 
 *      request: the HTTP request header struct
 *      buffer: the HTTP header buf
 *
 */
void get_first_line (struct HTTP_request* header, char *buffer){

    int line_length = strstr(buffer, "\r\n") - buffer;

    //create seperate buffer so strtok doesn't malform header string

    char line_buffer[line_length];
    bzero(line_buffer, line_length);
    strncpy(line_buffer, buffer, line_length);

    char *temp;

    temp = strtok(line_buffer, " ");
    header->type = calloc(strlen(temp) + 1, sizeof(char));
    strncpy(header->type, temp, strlen(temp));

    temp = strtok(NULL, " ");
    header->url = calloc(strlen(temp) + 1, sizeof(char));
    strncpy(header->url, temp, strlen(temp));


}

/* purpose: searches for a field in the HTTP header. If found, 'field' is pointed
 *          towards an allocated string containing the field contents.  If not found
 *          the pointer is pointed towards NULL
 * Args:
        field: a pointer to the struct member that needs to be populated
        field_string: the string search query in the HTTP header string
        buffer: contains the HTTP header string
 */

//TODO: if any of the if statements are triggered, probably should be an error logged instead of just setting it to NULL
void get_field(char **field, char *field_string, char *buffer){
    char* start_of_line = strstr(buffer, field_string);


    if (start_of_line == NULL){
        *field = NULL;
        return;
    } 
    // char* end_of_line = strstr(start_of_line, "\r\n");
    // if (end_of_line == NULL) return NULL;
    if (strstr(start_of_line, "\r\n") == NULL){
        *field = NULL;
        return;
    }

    int line_length = strstr(start_of_line, "\n") - start_of_line;

    //create seperate buffer so strtok doesn't malform header string

    char line_buffer[line_length];
    char *temp;
    bzero(line_buffer, line_length);
    strncpy(line_buffer, start_of_line, line_length);
    // fprintf(stderr, "start of line is %s\n", start_of_line);
    // fprintf(stderr, "line buffer is %s\n", line_buffer);
    // fprintf(stderr, "line length is %d\n", line_length);

    temp = strtok(line_buffer, " ");
    // fprintf(stderr, "temp is %s\n", temp);
    if (temp == NULL){
        // fprintf(stderr, "got set to null bc strtok(line_buffer, \" \") was null\n");
        *field = NULL;
        return;
    }
    temp = strtok(NULL, "\r");
    // fprintf(stderr, "temp is %s\n", temp);

    if (temp == NULL){
        // fprintf(stderr, "got set to null bc strtok(line_buffer, \"r\") was null\n");
        *field = NULL;
        return;
    }


    *field = calloc(strlen(temp) + 1, sizeof(char));
    strncpy(*field, temp, strlen(temp));

}