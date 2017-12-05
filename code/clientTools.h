#ifndef clientTools_h
#define clientTools_h

FILE * connectToServer(const char * hostname, const char * port);
uint32_t requestId(const char * hostname, const char * port);
void sortCsv(const char * path, uint32_t id, const char * hostname, const char * port);

#endif /* clientTools_h */
