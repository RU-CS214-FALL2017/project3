#include <strings.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

#include "serverTools.h"
#include "tools.h"

// Returns a unique generated ID.
uint32_t getNewRequestId() {
    
    static uint32_t requestId = 0;
    static pthread_mutex_t requestIdLock = PTHREAD_MUTEX_INITIALIZER;
    
    pthread_mutex_lock(&requestIdLock);
    uint32_t ret = requestId;
    requestId++;
    pthread_mutex_unlock(&requestIdLock);
    
    return ret;
}

// Handles an WACK init request.
void handleInit(FILE * client) {
    
    uint32_t netId = htonl(getNewRequestId());
    fwrite(&netId, sizeof(netId), 1, client);
}

void handleSort(FILE * client) {
    
    uint32_t net[2];
    fread(net, sizeof(uint32_t), 2, client);
    uint32_t id = ntohl(net[0]);
    uint32_t csvSize = ntohl(net[1]);
    
    printf("id: %u, sorted\n", id);
    struct table table;
    fillTable(client, csvSize, &table);
    
    FILE * out = fopen("test.csv", "w");
    printTable(out, table.table, table.numRows);
}

void handleRetr(FILE * stream) {
    printf("retr!\n");
}

// Handles a WACK request
void handleRequest(int connection, struct sockaddr_in * addr, socklen_t addrLen) {
    
    char * ip = inet_ntoa(addr->sin_addr);
    FILE * client = fdopen(connection, "r+");
    if (client == NULL) {
        fprintf(stderr, "Error opening fd\n");
        pthread_exit(NULL);
    }
   
    char requestType[5];
    fgets(requestType, 5, client);
    
    printf("%s\n", requestType);
    if (strcmp("init", requestType) == 0) {
        handleInit(client);
        
    } else if (strcmp("sort", requestType) == 0) {
        handleSort(client);
        
    } else if (strcmp("retr", requestType) == 0) {
        handleRetr(client);
        
    } else {
        fprintf(stderr, "Bad request from: %s\n", ip);
    }
    
    printf("done\n");
    fclose(client);
}

// Thread helper function for handleRequest.
void * handleRequestHelper(void * parameters) {
    
    struct handleRequestParams * params = parameters;
    handleRequest(params->connection, &(params->addr), params->addrLen);
    free(parameters);
    pthread_exit(NULL);
}

// Starts a sort server on <port>.
void startServer(const char * port) {
    
    struct sockaddr_in serverAddr;
    
    bzero(&serverAddr, sizeof(struct sockaddr_in));
    
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htons(INADDR_ANY);
    serverAddr.sin_port = htons(atol(port));
    
    int socketFd = socket(AF_INET, SOCK_STREAM, 0);
    
    if(bind(socketFd,(struct sockaddr *) &serverAddr, sizeof(struct sockaddr_in)) == -1) {
        
        fprintf(stderr, "Error binding to port %s\n", port);
        exit(EXIT_FAILURE);
    }
    
    if (listen(socketFd, 10) == -1) {
        
        fprintf(stderr, "Error listening on port %s\n", port);
        exit(EXIT_FAILURE);
    }
    
    while (1) {
        
        struct handleRequestParams * params = malloc(sizeof(struct handleRequestParams));
        params->addrLen = sizeof(params->addr);
        int acceptedFd = accept(socketFd, (struct sockaddr *) &(params->addr), &(params->addrLen));
        if (acceptedFd == -1) {
            fprintf(stderr, "Error accepting connection\n");
            exit(EXIT_FAILURE);
        }
        
        params->connection = acceptedFd;

        pthread_t tid;
        pthread_create(&tid, NULL, handleRequestHelper, params);
    }
}
