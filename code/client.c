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
#include "tools.h"

int main(int argc, char ** argv) {
    
    char * hostname = getHostname(argc, argv);
    char * port = getPort(argc, argv);
    char * header = getColumnHeader(argc, argv);
    
    initializeSockets(hostname, port, (unsigned int) atol(getPoolSize(argc, argv)));

    uint32_t id = requestId(header);
    
    code(sortCsv("movie_metadata.csv", id));
    retrieveCsv(id, "sorted.csv");
    closeSockets();
}
