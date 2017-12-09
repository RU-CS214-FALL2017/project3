#ifndef clientTools_h
#define clientTools_h

FILE * connectToServer(const char * hostname, const char * port);
uint32_t requestId(const char * columnHeader);
void sortCsv(const char * path, uint32_t id);
void retrieveAndSaveCsv(uint32_t id, const char * path);

#endif /* clientTools_h */
