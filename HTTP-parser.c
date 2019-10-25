#include "HTTP-parser.h"

#define HTTP_ENDL "\r\n"
#define INITIAL_RESPONSE_LENGTH 100

void get_first_line (struct HTTP_request* header, char *buffer);
void get_field(char **field, char *field_string, char *buffer);
int  add_status( char *status, char **buf_ptr, unsigned *buf_position, int buf_len);
int  add_field(char *field_type, char* field_content, char **buf_ptr, unsigned *buf_position, int buf_len);
int  add_endl(char** buffer, unsigned *buf_position, int buf_len);

//TODO: maybe make add_status and add_endl the same function with an arg for what to add

struct HTTP_request* parse_request(char* buffer){
    struct HTTP_request* header = malloc(sizeof(struct HTTP_request));

    get_first_line(header, buffer);
    get_field(&(header->host), "Host: ", buffer);
    // can add more fields here

    return header;
}
int make_HTTP_response_header(struct HTTP_response response_header, char** buffer){
    *buffer = calloc(INITIAL_RESPONSE_LENGTH, 1);
    unsigned buf_position = 0;
    int buf_len = INITIAL_RESPONSE_LENGTH;
    //edge case out adding first line, the stat
    buf_len = add_status(response_header.status, buffer, &buf_position, INITIAL_RESPONSE_LENGTH);

    buf_len = add_field("Server", response_header.server ,buffer,  &buf_position, buf_len);
    buf_len = add_field("Content-Length", response_header.content_length ,buffer,  &buf_position, buf_len);
    buf_len = add_field("Date", response_header.date ,buffer,  &buf_position, buf_len);
    buf_len = add_field("Content-Type", response_header.content_type ,buffer,  &buf_position, buf_len);
    buf_len = add_endl(buffer, &buf_position, buf_len);
    return buf_position;
}

void free_HTTP_request(struct HTTP_request* request){
    free(request->type);
    free(request->url);
    free(request->host);
    free(request);
    
}

/*
 * Purpose: adds the first line of the HTTP response header, the status code
 * Args: 
 *      status: a string containing the status code and response
 *      buffer: a pointer to the HTTP response string
 *      buf_position: a pointer to the value containing the current length
 *                    of the HTTP-response string. Should always start at 0.  Will be modified
 *                    to reflect new length at the end of function
 *      buf_len: the maximum length of the buffer
 * Returns: The maximum length of the buffer, which will be equal to or greater
            than the argument buf_len
 */
int  add_status( char *status, char **buf_ptr, unsigned *buf_position, int buf_len){

    char* buffer = *buf_ptr;
    //if not enough space in the buffer, double its size
    if (strlen(status) > buf_len - *buf_position){
        *buf_ptr = realloc(*buf_ptr, buf_len * 2);
        buffer = *buf_ptr;
        bzero(&(buffer[buf_len]), buf_len);
        buf_len = buf_len * 2;
    }

    
    strcpy(&(buffer[*buf_position]), status);
    *buf_position += strlen(status);
    return add_endl(buf_ptr, buf_position, buf_len);
}

/*
 * Purpose: adds a line to the HTTP response header
 * Args: 
 *      field_type: a string containing the type of field (content-length, DATE, etc.) that we will be adding
 *      field-content: a string containing the value of this field
 *      buffer: a pointer to the HTTP response string
 *      buf_position: a pointer to the value containing the current length
 *                    of the HTTP-response string. should be > 0.  Will be modified
 *                    to reflect new length at the end of function
 *      buf_len: the maximum length of the buffer
 * Returns: The maximum length of the buffer, which will be equal to or greater
            than the argument buf_len
 */
int  add_field(char *field_type, char* field_content, char **buf_ptr, unsigned *buf_position, int buf_len){
    
    //if the field_content is NULL or empty, don't add it
    if (field_content == NULL || strcmp(field_content, "") == 0) return buf_len; 
    //TODO: add an error logging for the above line
    char* seperator = ": ";
    char* buffer = *buf_ptr;

    //if not enough space in the buffer, double its size
    if (strlen(field_type) + strlen(field_content) + strlen(seperator) > buf_len - *buf_position){
        *buf_ptr = realloc(*buf_ptr, buf_len * 2);
        buffer = *buf_ptr;
        bzero(&(buffer[buf_len]), buf_len);
        buf_len = buf_len * 2;
    }

    strcpy(&(buffer[*buf_position]), field_type);
    *buf_position += strlen(field_type);
    strcpy(&(buffer[*buf_position]), seperator);
    *buf_position += strlen(seperator);
    strcpy(&(buffer[*buf_position]), field_content);
    *buf_position += strlen(field_content);
    return add_endl(buf_ptr, buf_position, buf_len);;


}

/*
 * Purpose: adds an HTTP endline, "\r\n"
 * Args: 
 *      buffer: a pointer to the HTTP response string
 *      buf_position: a pointer to the value containing the current length
 *                    of the HTTP-response string. should be > 0.  Will be modified
 *                    to reflect new length at the end of function by iterating by 2
 *      buf_len: the maximum length of the buffer
 * Returns: The maximum length of the buffer, which will be equal to or greater
            than the argument buf_len
 */

int  add_endl(char** buf_ptr, unsigned *buf_position, int buf_len){

    char* buffer = *buf_ptr;
    if (strlen(HTTP_ENDL) > buf_len - *buf_position){
        *buf_ptr = realloc(*buf_ptr, buf_len * 2);
        buffer = *buf_ptr;
        bzero(&(buffer[buf_len]), buf_len);
        buf_len = buf_len * 2;
    }

    
    
    strcpy(&(buffer[*buf_position]), HTTP_ENDL);
    *buf_position += strlen(HTTP_ENDL);
    return buf_len;
    
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
//TODO: if there's an error, maybe have it write an error message and have it return up the stack
void get_field(char **field, char *field_string, char *buffer){
    char* start_of_line = strstr(buffer, field_string);
    //if field does not exist
    if (start_of_line == NULL){
        *field = NULL;
        return;
    } 

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
    if (temp == NULL){
        *field = NULL;
        return;
    }
    temp = strtok(NULL, "\r");

    if (temp == NULL){
        *field = NULL;
        return;
    }


    *field = calloc(strlen(temp) + 1, sizeof(char));
    strncpy(*field, temp, strlen(temp));

}