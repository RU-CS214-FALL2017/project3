#include <strings.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Connects <socket> to <hostname> at <port>. On error, prints message
// and exits.
void connectToServer(int socket, const char * hostname, const char * port) {
    
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
    
    while (connect(socket, temp->ai_addr, sizeof(struct sockaddr)) == -1) {
        
        if (temp->ai_next == NULL) {
            
            fprintf(stderr, "Error connecting to %s on port %s\n", hostname, port);
            exit(EXIT_FAILURE);
        }
        
        temp = temp->ai_next;
    }
    
    freeaddrinfo(addr);
}

void test(int acceptedFd) {
        
    char buff[100];
    bzero(&buff, 100);
    read(acceptedFd, &buff, 100);
    printf("%s\n", buff);
    fflush(stdout);
}

void * testHelper(void * param) {
    
    int acceptedFd = (int) param;
    
    test(acceptedFd);
    
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
