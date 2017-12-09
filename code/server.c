#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mainTools.h"
#include "serverTools.h"
#include "tools.h"

int main(int argc, char ** argv) {
    
    char * port = getPort(argc, argv);
    printf("Connections: ");
    startServer(port);
}
