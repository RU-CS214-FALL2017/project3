#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <strings.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "mainTools.h"
#include "socketTools.h"

int main(int argc, char ** argv) {
    
    char * hostname = getHostname(argc, argv);
    char * port = getPort(argc, argv);
    
    int sockFd = socket(AF_INET, SOCK_STREAM, 0);
    
    connectToServer(sockFd, hostname, port);
    
    char buff[100];
    bzero(buff, 100);
    fgets(buff, 100, stdin);
    write(sockFd, buff, 100);
}
