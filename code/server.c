#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "mainTools.h"

int main(int argc, char ** argv) {
    
    char * port = getPort(argc, argv);
    int socketFd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverAddr;
    
    bzero(&serverAddr, sizeof(struct sockaddr_in));
    
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htons(INADDR_ANY);
    serverAddr.sin_port = htons(atol(port));
    
    if(bind(socketFd,(struct sockaddr *) &serverAddr, sizeof(struct sockaddr_in)) == -1) {
        
        fprintf(stderr, "Error binding to port %s\n", port);
        exit(EXIT_FAILURE);
    }
    
    if(listen(socketFd, 10) == -1) {
        
        fprintf(stderr, "Error listening on port %s\n", port);
        exit(EXIT_FAILURE);
    }
    
    int acceptedFd = accept(socketFd, NULL, NULL);
    if (acceptedFd == -1) {
        
        fprintf(stderr, "Error accepting connection\n");
        exit(EXIT_FAILURE);
    }
    
    char buff;
    
    for(int i = 0; i < 100; i++) {
        
        bzero(&buff, 1);
        read(acceptedFd, &buff, 1);
        printf("%c", buff);
    }
}
