#include <sys/socket.h>
#include <stdio.h>

#include "mainTools.h"
#include "serverTools.h"

int main(int argc, char ** argv) {
    
    char * port = getPort(argc, argv);
    
    startServer(port);
}
