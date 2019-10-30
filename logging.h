

/*
 * These functions log incoming connections, HTTP reqeusts/responses, and TCP connections respectively
 * Args: sockfd: the socket of the client the program is dealing with
 *
 */
void log_error(int sockfd, char* error);
void log_request(int sockfd, char* request);
void log_response(int sockfd, char* response);
void log_connection(int sockfd);

/*
 * Purpose: creates an HTTP-compliant date of the current moment
 *          and loads it into date_buffer
 * Args: An array of MAX_FIELD_SIZE bytes loaded with null bytes
 */

void format_date(char* date_buffer);
