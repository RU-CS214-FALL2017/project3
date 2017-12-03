#ifndef tools_h
#define tools_h

void connectToServer(int socket, const char * hostname, const char * port);
void startServer(int socket, const char * port);

#endif /* tools_h */
