#include <strings.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void * testHelper(void * param) {
    
    int acceptedFd = (int) param;
    
    
    FILE * input = fdopen(acceptedFd, "r");
    
    char buff[100];
    
    fgets(buff, 100, input);
    
    printf("%s\n", buff);
    
    fgets(buff, 100, input);
    
    printf("%s\n", buff);
    
    pthread_exit(NULL);
}

void startServer(int socket, const char * port) {
    
    struct sockaddr_in serverAddr;
    
    bzero(&serverAddr, sizeof(struct sockaddr_in));
    
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htons(INADDR_ANY);
    serverAddr.sin_port = htons(atol(port));
    
    if(bind(socket,(struct sockaddr *) &serverAddr, sizeof(struct sockaddr_in)) == -1) {
        
        fprintf(stderr, "Error binding to port %s\n", port);
        exit(EXIT_FAILURE);
    }
    
    while (listen(socket, 10) == 0) {

        struct sockaddr_in addr;
        socklen_t addrLen = sizeof(addr);
        
        int acceptedFd = accept(socket, (struct sockaddr *) &addr, &addrLen);
        printf("client: %u\n", addr.sin_addr.s_addr);
        if (acceptedFd == -1) {
            fprintf(stderr, "Error accepting connection\n");
            exit(EXIT_FAILURE);
        }

        pthread_t tid;
        pthread_create(&tid, NULL, testHelper, (void *) acceptedFd);
    }
    
    fprintf(stderr, "Error listening on port %s\n", port);
    exit(EXIT_FAILURE);
}
