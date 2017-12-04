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
