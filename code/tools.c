#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#include "tools.h"

// Prints appropriate message for <code> returned from
// a sort request.
void csvCodePrint(char code, const char * path) {
    
    switch (code) {
            
        case SUCCESS:
            printf("Successfully sorted: %s\n", path);
            break;
        case COLUMN_HEADER_NOT_FOUND:
            fprintf(stderr, "Error sorting, header not found: %s\n", path);
            break;
        case ID_NOT_FOUND:
            fprintf(stderr, "Error sorting, ID not found: %s\n", path);
            break;
        case TABLE_INCOMPATIBLE:
            fprintf(stderr, "Error sorting, incompatible: %s\n", path);
            break;
        default:
            fprintf(stderr, "Error sorting: %s\n", path);
            break;
    }
}

// Removes the characters from <str> between indexes <startIndex> (inclusive)
// and <endIndex> (exclusive).
void removeChars (char * str, unsigned long startIndex, unsigned long endIndex) {
    
    unsigned long terminatingIndex = strlen(str);
    
    for(int i = 0; i <= (terminatingIndex - endIndex); i++) {
        
        str[startIndex + i] = str[endIndex + i];
    }
}

// Removes leading and trailing whitespaces from <str>.
void trim (char * str) {
    
    int i = 0;
    
    while(str[i] == ' ' || str[i] == '\n' || str[i] == '\t' || str[i] == '\r' || str[i] == '\v'
          || str[i] == '\f') {
        i += 1;
    }
    
    removeChars(str, 0, i);
    
    unsigned long terminatingIndex = strlen(str);
    unsigned long j = terminatingIndex - 1;
    
    while(str[j] == ' ' || str[j] == '\n' || str[j] == '\t' || str[j] == '\r' || str[j] == '\v'
          || str[j] == '\f') {
        j -= 1;
    }
    
    removeChars(str, j + 1, terminatingIndex);
}

// Parses <line>, a row from a CSV file, creates a 2D string
// array and has *<row> point to it.
unsigned int tokenizeRow(const char * line, char * ** row) {
    
    if (row != NULL) {
         *row = malloc(strlen(line) * sizeof(char *));
    }
    
    char tempChar = '\0';
    char tempCell[TEMPSIZE];
    int i = 0; // number of columns
    int j = 0; // number of characters in column field
    int inQuote = 0;
    int outQuote = 0;
    
    for(int l = 0; l <= strlen(line); l ++) {
        
        tempChar = line[l];
        
        if ((tempChar == ',' || tempChar == '\0') && !inQuote){
            
            tempCell[j] = '\0';
            trim(tempCell);
            if (row != NULL) {
                (*row)[i] = malloc(strlen(tempCell) + 1);
                strcpy((*row)[i], tempCell);
            }
            
            j = 0;
            i += 1;
            
            outQuote = 0;
            
        } else if (tempChar == '"' && inQuote) {
            inQuote = 0;
            
        } else if (tempChar == '"' && !outQuote && !inQuote) {
            inQuote = 1;
            
        } else {
            
            tempCell[j] = tempChar;
            j += 1;
            if(tempChar != ' '){
                outQuote = 1;
            }
        }
    }
    
    *row = realloc(*row, i * sizeof(char *));
    
    return i;
}

// Returns 1 if both tables have the same headers, else 0.
int sameHeaders(struct Table * table1, struct Table * table2) {

    if (table1->columns != table2->columns) {
        return 0;
    }
    
    for (int i = 0; i < table1->columns; i++) {
        
        if (strcmp((table1->table)[0][i], (table2->table)[0][i]) != 0) {
            return 0;
        }
    }
    
    return 1;
}



// Frees <table>.
void freeTable(struct Table * table) {
    
    for (int i = 0; i < table->rows; i++) {
        
        for (int j = 0; i < table->columns; j++) {
            free((table->table)[i][j]);
        }
        
        free((table->table)[i]);
    }
    
    free(table->table);
    free(table);
}

// Frees a row form <table> at the row's <index>.
void freeRow(struct Table * table, unsigned int index) {
    
    for (int i = 0; i < table->columns; i++) {
        free((table->table)[index][i]);
    }
    
    free((table->table)[index]);
}

// Fills *<table>.table with data from <stream> of <size> bytes.
// Sets *<tabe>'s rows and columns. Skips rows that don't have
// the same number of columns as the header.
void fillTable(FILE * stream, uint32_t size, struct Table * table) {
    
    table->table = malloc(TEMPSIZE * TEMPSIZE * sizeof(char **));
    char line[TEMPSIZE];
    table->rows = 0;
    table->columns = 0;
    
    do {
        
        fgets(line, TEMPSIZE, stream);
        
        int tempColumns = tokenizeRow(line, &(table->table)[table->rows]);
        
        if (table->rows == 0) {
            
            table->columns = tempColumns;
            (table->rows)++;
            
        } else if (tempColumns == table->columns) {
            (table->rows)++;
            
        } else {
            freeRow(table, table->rows);
        }
        
        size -= strlen(line);
        
    } while (size > 0);
    
    table->table = realloc(table->table, sizeof(char **) * table->rows);
}

// Prints <table> in a CSV format to <stream>.
void printTable (FILE * stream, struct Table * table) {
    
    for (int i = 0; i < table->rows; i++) {
        
        if(strchr((table->table)[i][0], ',') != NULL) {
            fprintf(stream, "\"%s\"", (table->table)[i][0]);
            
        } else {
            fprintf(stream, "%s", (table->table)[i][0]);
        }
        
        for (int j = 1; j < table->columns; j++) {
            
            if(strchr((table->table)[i][j], ',') != NULL) {
                fprintf(stream, ",\"%s\"", (table->table)[i][j]);
                
            } else {
                fprintf(stream, ",%s", (table->table)[i][j]);
            }
        }
        
        fprintf(stream, "\n");
    }
}

// Returns the size in bytes of <table> if it were printed
// as a CSV.
uint32_t printedSizeOfTable(struct Table * table) {
    
    uint32_t size = 0;
    char temp[TEMPSIZE];
    
    for (int i = 0; i < table->rows; i++) {
        
        if(strchr((table->table)[i][0], ',') != NULL) {
            size += sprintf(temp, "\"%s\"", (table->table)[i][0]);
            
        } else {
            size += sprintf(temp, "%s", (table->table)[i][0]);
        }
        
        for (int j = 1; j < table->columns; j++) {
            
            if(strchr((table->table)[i][j], ',') != NULL) {
                size += sprintf(temp, ",\"%s\"", (table->table)[i][j]);
                
            } else {
                size += sprintf(temp, ",%s", (table->table)[i][j]);
            }
        }
        
        size += sprintf(temp, "\n");
    }
    
    return size;
}

// Returns 1 if <str> is a valid number, else returns 0.
int isNumber(const char * str) {
    
    char * end = 0;
    strtod(str, &end);
    
    for (int i = 0; i < strlen(str); i++) {
        
        if (end == str + i) {
            return 0;
        }
    }

    return 1;
}

// If <areNumbers> is set to 0, returns 0 if <y> is lexicographically before <x>,
// else returns 1. If <areNumbers> is set to anything besides 0, converts <x> and
// <y> to double values x y respectively, and returns x <= y.
int isXBeforeY (const char * x, const char * y, int isNumeric) {
    
    if (isNumeric) {
        return atof(x) <= atof(y);
        
    } else {
        return (strcmp(x, y) <= 0);
    }
}

// If all the values in <table> at column index <columnIndex> from row index 1
// to row index <rows> - 1 are valid numbers, returns 1, else returns 0.
int isNumericColumn(struct Table * table, int columnIndex) {
    
    for (int i = 1; i < table->rows; i++) {
        
        if (!isNumber((table->table)[i][columnIndex])) {
            return 0;
        }
    }
    
    return 1;
}

// Returns 1 if <csvPath> ends in ".csv", else returns 0.
int isCsv(const char * csvPath) {
    
    const char * extension = csvPath + strlen(csvPath) - 4;
    
    if (!strcmp(".csv", extension)) {
        return 1;
    }
    
    return 0;
}

// Returns the index of <columnHeader> in <table> with <columns>
// columns. If <columnHeader> not found, return -1.
int getColumnHeaderIndex(const char * columnHeader, struct Table * table) {
    
    for (int i = 0; i < table->columns; i++) {
        
        if (!strcmp(columnHeader, (table->table)[0][i])) {
            return i;
        }
    }
    
    return -1;
}
