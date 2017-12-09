#ifndef clientTools_h
#define clientTools_h

#include <stdio.h>
#include <inttypes.h>

FILE * connectToServer(const char * hostname, const char * port);
uint32_t requestId(const char * columnHeader);
void sortCsv(const char * path, uint32_t id);
void retrieveAndSaveCsv(uint32_t id, const char * path, const char * columnHeader);

#endif /* clientTools_h */
