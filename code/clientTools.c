#include <netdb.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/sendfile.h>
#include <string.h>

#include "tools.h"
#include "socketPool.h"

//  Connects to <hostname> at <port>. On error, prints message
// and exits, else returns the open socket.
FILE * connectToServer(const char * hostname, const char * port) {
    
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
    int sockFd = socket(AF_INET, SOCK_STREAM, 0);
    
    while (connect(sockFd, temp->ai_addr, sizeof(struct sockaddr)) == -1) {
        
        if (temp->ai_next == NULL) {
            
            fprintf(stderr, "Error connecting to %s on port %s\n", hostname, port);
            exit(EXIT_FAILURE);
        }
        
        temp = temp->ai_next;
    }
    
    freeaddrinfo(addr);
    
    return fdopen(sockFd, "r+");
}

// Requests and returns a unique ID from the server. The
// server instantiates the ID with <columnHeader>.
uint32_t requestId(const char * columnHeader) {
    
    unsigned long headerLength = strlen(columnHeader);
    uint32_t netHeaderLength = htonl(headerLength);

    FILE * server = getSocket();
    
    fwrite("init", 4, 1, server);
    fwrite(&netHeaderLength, 4, 1, server);
    fwrite(columnHeader, headerLength, 1, server);
    
    uint32_t netId;
    fread(&netId, sizeof(netId), 1, server);
    
    returnSocket(server);
    
    return ntohl(netId);
}

// Sends CSV file at <path> to the server for sorting in <id>'s
// space. Prints the status of the sorted file.
void sortCsv(const char * path, uint32_t id) {
    
    if (!isCsv(path)) {
        
        fprintf(stderr, "Not a CSV: %s\n", path);
        return;
    }
    
    struct stat fileInfo;
    stat(path, &fileInfo);
    uint32_t net[2];
    uint32_t size = (uint32_t) fileInfo.st_size;
    net[0] = htonl(id);
    net[1] = htonl(size);

    FILE * csv = fopen(path, "r");
    char sorted;
    
    FILE * server = getSocket();

    fwrite("sort", 4, 1, server);
    fwrite(&net, 4, 2, server);
    fflush(server);
    sendfile(fileno(server), fileno(csv), NULL, size);

    fread(&sorted, 1, 1, server);

    returnSocket(server);
    
    fclose(csv);
    csvCodePrint(sorted, path);
}

// Retrieves the sorted CSV of all CSVs sorted with <id>. Saves
// the file to "<path>/AllFiles-sorted-<columnHeader>.csv".
void retrieveAndSaveCsv(uint32_t id, const char * path, const char * columnHeader) {
    
    uint32_t netId = htonl(id);
    uint32_t netSize;
    
    FILE * server = getSocket();
    
    fwrite("retr", 4, 1, server);
    fwrite(&netId, 4, 1, server);
    
    fread(&netSize, 4, 1, server);
    uint32_t size = ntohl(netSize);
    
    if (size > 0)  {
        
        char filePath[strlen(path) + strlen(columnHeader) + 22];
        sprintf(filePath, "%s/AllFiles-sorted-%s.csv", path, columnHeader);
        
        FILE * csv = fopen(filePath, "w");
        char temp[TEMPSIZE];
        
        while (size > 0) {
            
            size -= strlen(fgets(temp, TEMPSIZE, server));
            fprintf(csv, "%s", temp);
        }
        
        fclose(csv);
    }
    
    returnSocket(server);
}
