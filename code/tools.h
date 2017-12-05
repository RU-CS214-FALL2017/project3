#ifndef tools_h
#define tools_h

#define TEMPSIZE 4096
#define COLUMNS 28

#include <stdio.h>
#include <stdint.h>

extern unsigned int AllThreadsCount;

struct table {
  
    char *** table;
    char * ** rowsMems;
    char * * cellsMems;
    unsigned int numRows;
    unsigned int numRowsMems;
    unsigned int numCellsMems;
};

void * processCsvDir(void * param);
unsigned int getIndex(const char * header);
void checkDir(const char * path, const char * dirType);
void printToSortedCsv(struct table * table);
int isCsv(const char * csvPath);
int isXBeforeY (const char * x, const char * y);
int fillTable(FILE * csv, uint32_t csvSize, struct table * table);
void printTable (FILE * stream, char *** table, unsigned int rows);
void freeTable(struct table * table);

#endif /* tools_h */
