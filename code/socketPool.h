#ifndef socketPool_h
#define socketPool_h

void initializeSockets(const char * hostname, const char * port, unsigned int num);
FILE * getSocket(void);
void returnSocket(FILE * socket);
void closeSockets(void);

#endif /* socketPool_h */
