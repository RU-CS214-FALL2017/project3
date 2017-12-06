#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>

#include "mainTools.h"
#include "clientTools.h"
#include "socketPool.h"

int main(int argc, char ** argv) {
    
    char * hostname = getHostname(argc, argv);
    char * port = getPort(argc, argv);
    
    initializeSockets(hostname, port, (unsigned int) atol(getPoolSize(argc, argv)));

   
    for(int i = 0; i < 10; i++) {
        
        uint32_t id = requestId();
        printf("id: %u\n", id);
    }
    
    closeSockets();
}
