#ifndef socketPool_h
#define socketPool_h

FILE * getSocket(void);
void returnSocket(FILE * socket);
void initializeSockets(const char * hostname, const char * port, unsigned int num);
void closeSockets(void);

#endif /* socketPool_h */
