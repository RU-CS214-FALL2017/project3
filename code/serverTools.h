#ifndef serverTools_h
#define serverTools_h

#include <netinet/in.h>

struct handleRequestParams {
  
    int connection;
    struct sockaddr_in addr;
    socklen_t addrLen;
};

void startServer(const char * port);

#endif /* serverTools_h */
