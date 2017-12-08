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
#include "sorter.h"
#include "csvStore.h"

void handleRequest(FILE * client, const char * ip);

// Returns a unique generated ID.
uint32_t getNewId() {
    
    static uint32_t id = 0;
    static pthread_mutex_t idLock = PTHREAD_MUTEX_INITIALIZER;
    
    pthread_mutex_lock(&idLock);
    uint32_t ret = id;
    id++;
    pthread_mutex_unlock(&idLock);
    
    return ret;
}

// Handles an WACK init request.
void handleInit(FILE * client, const char * ip) {
    
    uint32_t netColumnHeaderLength;
    fread(&netColumnHeaderLength, 4, 1, client);
    uint32_t columnHeaderLength = ntohl(netColumnHeaderLength);
    printf("header length: %u\n", columnHeaderLength);
    
    char * columnHeader = malloc(columnHeaderLength + 1);
    fgets(columnHeader, columnHeaderLength + 1, client);
    printf("Recieved column header: %s\n", columnHeader);
    
    uint32_t id = getNewId();
    initializeId(id, columnHeader);
    uint32_t netId = htonl(id);
    fwrite(&netId, 4, 1, client);
    
    handleRequest(client, ip);
}


void handleSort(FILE * client, const char * ip) {
    
    uint32_t net[2];
    fread(net, 4, 2, client);
    uint32_t id = ntohl(net[0]);
    uint32_t csvSize = ntohl(net[1]);

    printf("csvSize: %u\n", csvSize);
    printf("id: %u, sorting\n", id);
    struct Table * table = malloc(sizeof(struct Table));
    fillTable(client, csvSize, table);
    printf("table filled\n");
    checkHeaders(table, id);
    sortById(id, table);
    printf("table sorted\n");
    char sorted = 1;
    fwrite(&sorted, 1, 1, client);
    fflush(client);

    FILE * out = fopen("recieved.csv", "w");
    printTable(out, table);
    fclose(out);
    
    handleRequest(client, ip);
}

// Handles a retr request.
void handleRetr(FILE * client, const char * ip) {
    
    uint32_t netId;
    fread(&netId, 4, 1, client);
    uint32_t id = ntohl(netId);
    printf("dump id: %u\n", id);
    
    struct Table * table = dumpTable(id);
    printf("got table\n");
    FILE * out = fopen("sorted.csv", "w");
    printTable(out, table);
    fclose(out);
    printf("printed sorted\n");
    
    handleRequest(client, ip);
}

// Handles a done request.
void handleDone(FILE * client, const char * ip) {
    
    printf("closing connection\n");
    fclose(client);
}

// Handles a WACK request
void handleRequest(FILE * client, const char * ip) {
   
    char requestType[5];
    fgets(requestType, 5, client);
    
    printf("requestType %s\n", requestType);
    if (strcmp("init", requestType) == 0) {
        handleInit(client, ip);
        
    } else if (strcmp("sort", requestType) == 0) {
        handleSort(client, ip);
        
    } else if (strcmp("retr", requestType) == 0) {
        handleRetr(client, ip);
        
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
