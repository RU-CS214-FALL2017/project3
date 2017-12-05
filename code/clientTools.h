#ifndef clientTools_h
#define clientTools_h

FILE * connectToServer(const char * hostname, const char * port);
uint32_t requestId(void);
void sortCsv(const char * path, uint32_t id);

#endif /* clientTools_h */
