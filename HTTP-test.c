#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include "HTTP-parser.h"
#include "io.h"

#define DEFAULT_BUFFER_SIZE 10000


int main (int argc, char** argv){
    if (argc < 2){
        error("please specify filename(s) to read sample HTTP header from");
    }
    for (int i = 1; i < argc; i++){
        FILE *fp = fopen(argv[i], "rb");
        if (fp == NULL){
            fprintf(stderr, "failed to open file \"%s\", it probably doesn't exist\n", argv[i]);
        }
        fseek(fp, 0, SEEK_END);
        long fsize = ftell(fp);
        fseek(fp, 0, SEEK_SET);  /* same as rewind(f); */

        char buf[fsize];
        fread(buf, 1, fsize, fp);
        struct HTTP_request* header = parse_request(buf);
        fclose(fp);

        fprintf(stderr, "in main, type is \"%s\"\n", header->type);
        fprintf(stderr, "in main, url is \"%s\"\n", header->url);
        fprintf(stderr, "in main, host is \"%s\"\n", header->host);

    }

    return 0;
}