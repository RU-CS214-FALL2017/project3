#include <strings.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "tools.h"
#include "socketPool.h"

// Connects <socket> to <hostname> at <port>. On error, prints message
// and exits.
FILE * connectToServer(const char * hostname, const char * port) {
    
    struct addrinfo hints;
    bzero(&hints, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_V4MAPPED;
    
    struct addrinfo * addr;
    
    if (getaddrinfo(hostname, port, &hints, &addr) != 0) {
        
        fprintf(stderr, "Error finding IPv4 address of %s on port %s\n", hostname, port);
        exit(EXIT_FAILURE);
    };
    
    struct addrinfo * temp = addr;
    int sockFd = socket(AF_INET, SOCK_STREAM, 0);
    
    while (connect(sockFd, temp->ai_addr, sizeof(struct sockaddr)) == -1) {
        
        if (temp->ai_next == NULL) {
            
            fprintf(stderr, "Error connecting to %s on port %s\n", hostname, port);
            exit(EXIT_FAILURE);
        }
        
        temp = temp->ai_next;
    }
    
    freeaddrinfo(addr);
    
    return fdopen(sockFd, "r+");
}

uint32_t requestId() {
    
    FILE * server = getSocket();
    
    fprintf(server, "init");
    uint32_t netId;
    fread(&netId, sizeof(netId), 1, server);
    
    returnSocket(server);
    
    return ntohl(netId);
}

void sortCsv(const char * path, uint32_t id) {
    
    FILE * server = getSocket();
    
    fwrite("sort", 4, 1, server);
    
    struct stat fileInfo;
    stat(path, &fileInfo);
    uint32_t net[2];
    uint32_t size = (uint32_t) fileInfo.st_size;
    net[0] = htonl(id);
    net[1] = htonl(size);
    
    fwrite(&net, sizeof(uint32_t), 2, server);
    
    FILE * csv = fopen(path, "r");
    char temp[TEMPSIZE];
    
    while (fgets(temp, TEMPSIZE, csv) != NULL) {
        fprintf(server, "%s", temp);
    }
    
    returnSocket(server);
}
