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

int main(int argc, char ** argv) {
    
    char * hostname = getHostname(argc, argv);
    char * port = getPort(argc, argv);

   
    uint32_t id = requestId(hostname, port);
    printf("id: %u\n", id);
    sortCsv("movie_metadata.csv", id, hostname, port);
}
