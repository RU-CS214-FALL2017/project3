#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "mainTools.h"

int main(int argc, char ** argv) {
    
    char * hostname = getHostname(argc, argv);
    char * port = getPort(argc, argv);
    
    struct addrinfo hints;
    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_V4MAPPED;
    
    struct addrinfo * addr;
    
    if (getaddrinfo(hostname, port, &hints, &addr) != 0) {
        
        fprintf(stderr, "Error finding ip address");
        exit(EXIT_FAILURE);
    };
    
    int sockFd = socket(AF_INET, SOCK_STREAM, 0);
    if (connect(sockFd, addr->ai_addr, sizeof(struct sockaddr)) == -1) {
        
        fprintf(stderr, "Error connecting");
        exit(EXIT_FAILURE);
    }
    
    char send[100];
    
    bzero(send, 100);
    fgets(send, 100, stdin);
    write(sockFd, send, 100);
    
}
