#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "mainTools.h"
#include "clientTools.h"

int main(int argc, char ** argv) {
    
    char * hostname = getHostname(argc, argv);
    char * port = getPort(argc, argv);
    
    int sockFd = socket(AF_INET, SOCK_STREAM, 0);
    
    connectToServer(sockFd, hostname, port);

    FILE * file = fdopen(sockFd, "w");
    
    fprintf(file, "hello\nman\n");
    
}
