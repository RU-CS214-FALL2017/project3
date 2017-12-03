#include <sys/socket.h>

#include "mainTools.h"
#include "socketTools.h"

int main(int argc, char ** argv) {
    
    char * port = getPort(argc, argv);
    int socketFd = socket(AF_INET, SOCK_STREAM, 0);
    
    startServer(socketFd, port);
}
