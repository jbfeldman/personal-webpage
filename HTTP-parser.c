#include "HTTP-parser.h"

struct HTTP_request{
    char *get;
    char *host;
    /*char* conncetion;
    char *cache_control;
    char *user_agent; */
};
struct HTTP_response{
    char *status;
    /*char *date;
    char *server;
    char *content_length; */
};

struct HTTP_request* parse_request(char* buffer, int n){
    ;
}
int make_HTTP_response_header(struct HTTP_response response_header, char** buffer){
    ;
}
void free_HTTP_request(struct HTTP_request* request){
    ;
}