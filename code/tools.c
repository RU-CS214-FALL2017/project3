#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

#include "tools.h"
//#include "forkTools.h"
//#include "memTools.h"

//int findCsvFilesHelper(const char * dirPath, char ** csvPaths, int * numFound);
//void printDirTreeHelper(FILE * output, pid_t pid, struct sharedMem * sharedMem, unsigned int level);

// <row> is the address to a char **. Creates a array of strings
// A, where each comma seperated value from <line> is an element
// of A, and <row>'s refrence is set to point to A. Returns the
// number elements in A (columns). To free, free each (*<row>)[i]
// (0 <= i < # of elements in A) and free *<row>.
unsigned int tokenizeRow(const char * line, char * ** row) {
    
    if (row != NULL) {
         *row = (char **) malloc(strlen(line) * sizeof(char *));
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
                (*row)[i] = (char *) malloc(strlen(tempCell) + 1);
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
    
    *row = (char **) realloc(*row, i * sizeof(char *));
    
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

// Removes the characters from <str> between indexes <startIndex> (inclusive)
// and <endIndex> (exclusive).
void removeChars (char * str, unsigned long startIndex, unsigned long endIndex) {
    
    unsigned long terminatingIndex = strlen(str);
    
    for(int i = 0; i <= (terminatingIndex - endIndex); i++) {
        
        str[startIndex + i] = str[endIndex + i];
    }
}

// Frees a table.
void freeTable(struct Table * table) {
    
    for (int i = 0; i < table->rows; i++) {
        
        for (int j = 0; i < table->columns; j++) {
            free((table->table)[i][j]);
        }
        
        free((table->table)[i]);
    }
    
    free(table->table);
}

// Frees a row.
void freeRow(struct Table * table, unsigned int index) {
    
    for (int i = 0; i < table->columns; i++) {
        free((table->table)[index][i]);
    }
    
    free((table->table)[index]);
}

// <table> is an address to a char ***. <rows> is an address to
// an unsigned int. <columns> is an address to an unsigned int.
// Creates a "table" from <csvFile> as a 2D string array, where
// A[n][m] will return a string representation of the data stored
// in the (n+1)th row and (m+1)th column of the "table". <table>'s
// refrence is set to point to the created "table". <rows>' refrence
// is set to the numbers of rows in "table". <columns>' refrence is
// set to the number of columns in "table". To free, free each
// (*<table>)[i][j] (0 <= i < *<rows>, 0 <= j < *<columns>) and free
// *<table>.
void fillTable(FILE * stream, uint32_t size, struct Table * table) {
    
    table->table = (char ***) malloc(TEMPSIZE * TEMPSIZE * sizeof(char **));
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
    
    table->table = (char ***) realloc(table->table, sizeof(char **) * table->rows);
}

// Prints <table> with <rows> rows and <columns> columns in a
// csv (comma seperated values) format to <stream>.
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
int isXBeforeY (const char * x, const char * y, int areNumbers) {
    
    if (areNumbers) {
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

// Returns 1 if <csvPath> is a path to a "proper" .csv file, else returns 0.
int isCsv(const char * csvPath) {
    
    const char * extension = csvPath + strlen(csvPath) - 4;
    
    if (!strcmp(".csv", extension)) {
        return 1;
    }
    
    return 0;
}

// If the name of the CSV file located at <path> is A. This function returns
// the newly allocated string: "<outputDir>/A-sorted-<columnHeader>.csv". To
// free, free the returned pointer.
char * sortedCsvPath(const char * csvPath, const char * columnHeader, const char * outputDir) {
    
    char temp[strlen(csvPath) + 1];
    strcpy(temp, csvPath);
    
    char * token = strtok(temp, "/");
    char * tempTok = NULL;
    
    while (token != NULL) {
        
        tempTok = token;
        token = strtok(NULL, "/");
    }
    
    tempTok[strlen(tempTok) - 4] = '\0';
    
    char * ret = (char * ) malloc(strlen(outputDir) + strlen(tempTok) + strlen(columnHeader) + 14);
    sprintf(ret, "%s/%s-sorted-%s.csv", outputDir, tempTok, columnHeader);
    
    return ret;
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

//void printDirTree(FILE * output, struct sharedMem * sharedMem) {
//    printDirTreeHelper(output, getpid(), sharedMem, 0);
//}
//
//void printDirTreeHelper(FILE * output, pid_t pid, struct sharedMem * sharedMem, unsigned int level) {
//
//    struct csvDir * dir = getDirSeg(sharedMem, pid);
//
//    char * begin = "| ";
//    char * end = "|-";
//
//    for (int i = 0; i < level; i++){
//        fprintf(output, "%s", begin);
//    }
//
//    fprintf(output, "%s%d: Processed the directory %s\n", end, pid, dir->path);
//
//    for (int i = 0; i < dir->numCsvs; i++) {
//
//        for (int i = 0; i < (level + 1); i++){
//            fprintf(output, "%s", begin);
//        }
//
//        struct csv * csv = getCsvSeg(sharedMem, dir->csvPids[i]);
//
//        if(csv->sorted) {
//
//            fprintf(output, "%s%d: Sorted the file %s", end, (dir->csvPids)[i], csv->path);
//
//            if (csv->error) {
//                fprintf(output, " (%s)", csv->errors);
//            }
//
//            fprintf(output, "\n");
//
//        } else {
//
//            fprintf(output, "%s%d: Tried to sort the file %s (%s)\n", end, (dir->csvPids)[i], csv->path, csv->errors);
//        }
//    }
//
//    for (int i = 0; i < dir->numSubDirs; i++) {
//        printDirTreeHelper(output, (dir->subDirsPids)[i], sharedMem, level + 1);
//    }
//}

//unsigned int dirSubProcessCount(pid_t dirPid, struct sharedMem * sharedMem) {
//    
//    struct csvDir * dir = getDirSeg(sharedMem, dirPid);
//    
//    unsigned int count = 1 + dir->numCsvs;
//    
//    for (int i = 0; i < dir->numSubDirs; i++) {
//        count += dirSubProcessCount((dir->subDirsPids)[i], sharedMem);
//    }
//    
//    return count;
//}

// Checks weather <path> is a valid directory, if not,
// the program crashes with an approiate error message.
void checkDir(const char * path, const char * dirType) {

    DIR * dir = opendir(path);
    
    if (dir == NULL) {
        
        errno = 0;
        
        switch (errno) {
                
            case EACCES:
                printf("You do not have access to the specified %s directory, %s\n", dirType, path);
                break;
                
            case ENOENT:
                printf("The specified %s directory, %s, does not exist\n", dirType, path);
                break;
                
            case ENOTDIR:
                printf("The specified %s directory, %s, is not a directory\n", dirType, path);
                break;
                
            default:
                printf("A problem occured opening the specified %s directory, %s", dirType, path);
                break;
        }
        
        closedir(dir);
        exit(EXIT_FAILURE);
    }
    
    closedir(dir);
}

