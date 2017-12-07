#ifndef tools_h
#define tools_h

#include <stdio.h>
#include <sys/types.h>
#include <stdint.h>

#define TEMPSIZE 4098

struct Table {
    
    char *** table;
    unsigned int rows;
    unsigned int columns;
};

unsigned int tokenizeRow(const char * line, char * ** row);
int sameHeaders(struct Table * table1, struct Table * table2);
void trim (char * str);
void removeChars (char * str, unsigned long startIndex, unsigned long endIndex);
void fillTable(FILE * stream, uint32_t size, struct Table * table);
void printTable (FILE * stream, struct Table * table);
int isNumber(const char * str);
int isXBeforeY (const char * x, const char * y, int areNumbers);
int isNumericColumn(struct Table * table, int columnIndex);
int findCsvFiles(const char * dirPath, char * ** csvPaths, int * numFound);
int isCsv(const char * csvPath);
unsigned int lineageParser(const char * path, char * ** lineage);
char * sortedCsvPath(const char * csvPath, const char * columnHeader, const char * outputDir);
int getColumnHeaderIndex(const char * columnHeader, struct Table * table);
//void printDirTree(FILE * output, struct sharedMem * sharedMem);
//unsigned int dirSubProcessCount(pid_t dirPid, struct sharedMem * sharedMem);
void checkDir(const char * path, const char * dirType);

#endif /* tools_h */
