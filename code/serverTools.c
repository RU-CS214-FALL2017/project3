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

void handleRequest(FILE * client, const char * ip);

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
void handleInit(FILE * client, const char * ip) {
    
    uint32_t netId = htonl(getNewRequestId());
//    write(fileno(client), &netId, sizeof(netId));
    fwrite(&netId, sizeof(netId), 1, client);
    handleRequest(client, ip);
//    close(client);
}

void handleSort(FILE * client, const char * ip) {
    
//    uint32_t net[2];
//    fread(net, sizeof(uint32_t), 2, client);
//    uint32_t id = ntohl(net[0]);
//    uint32_t csvSize = ntohl(net[1]);
//
//    printf("id: %u, sorted\n", id);
//    struct table table;
//    fillTable(client, csvSize, &table);
//    fclose(client);
//
//    FILE * out = fopen("test.csv", "w");
//    printTable(out, table.table, table.numRows);
//    fclose(out);
}

void handleRetr(FILE * client, const char * ip) {
    
//    printf("retr!\n");
//    fclose(client);
}

void handleNone(FILE * client, const char * ip) {
    handleRequest(client, ip);
}

void handleDone(FILE * client, const char * ip) {
    printf("closing connection\n");
    fclose(client);
}

// Handles a WACK request
void handleRequest(FILE * client, const char * ip) {
   
    char requestType[5];
    fgets(requestType, 5, client);
//    recv(fileno(client), requestType, 4, MSG_WAITALL);
//    read(client, requestType, 4);
//    requestType[4] = '\0';
    
    printf("requestType %s\n", requestType);
    if (strcmp("init", requestType) == 0) {
        handleInit(client, ip);
        
    } else if (strcmp("sort", requestType) == 0) {
        handleSort(client, ip);
        
    } else if (strcmp("retr", requestType) == 0) {
        handleRetr(client, ip);
        
    } else if (strcmp("none", requestType) == 0) {
        handleNone(client, ip);
        
    } else if (strcmp("done", requestType) == 0) {
        handleDone(client, ip);
        
    } else {
        fprintf(stderr, "Bad request from: %s\n", ip);
        fclose(client);
    }
}

// Handles an accepted connection.
void acceptConnection(int connection, struct sockaddr_in * addr, socklen_t addrLen) {
    
    char * ip = inet_ntoa(addr->sin_addr);
    FILE * client = fdopen(connection, "r+");
    
    if (client == NULL) {

        fprintf(stderr, "Error opening file descriptor of socket from %s\n", ip);
        pthread_exit(NULL);
    }
    printf("accepted connection\n");
    handleRequest(client, ip);
}

// Thread helper function for handleRequest.
void * acceptConnectionHelper(void * parameters) {
    
    struct handleRequestParams * params = parameters;
    acceptConnection(params->connection, &(params->addr), params->addrLen);
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
    
    if (listen(socketFd, 100) == -1) {
        
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
        pthread_create(&tid, NULL, acceptConnectionHelper, params);
    }
}
