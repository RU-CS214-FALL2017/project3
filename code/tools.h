#ifndef tools_h
#define tools_h

#include <stdio.h>
#include <sys/types.h>
#include <stdint.h>

// A temporary size.
#define TEMPSIZE 4098

// Codes sent from the server or sort request.
#define SUCCESS 0
#define COLUMN_HEADER_NOT_FOUND 1
#define ID_NOT_FOUND 2
#define TABLE_INCOMPATIBLE 3

// Represents a table.
struct Table {
    
    char *** table;
    unsigned int rows;
    unsigned int columns;
};

void csvCodePrint(char code, const char * path);
unsigned int tokenizeRow(const char * line, char * ** row);
int sameHeaders(struct Table * table1, struct Table * table2);
void freeTable(struct Table * table);
void fillTable(FILE * stream, uint32_t size, struct Table * table);
void printTable (FILE * stream, struct Table * table);
uint32_t printedSizeOfTable(struct Table * table);
int isXBeforeY (const char * x, const char * y, int isNumeric);
int isNumericColumn(struct Table * table, int columnIndex);
int isCsv(const char * csvPath);
int getColumnHeaderIndex(const char * columnHeader, struct Table * table);

#endif /* tools_h */
