#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <pthread.h>

#include "tools.h"
#include "sorter.h"

unsigned int AllThreadsCount = 2;
pthread_mutex_t ATCM = PTHREAD_MUTEX_INITIALIZER;

// Headers of a proper movie_metadata CSV.
const char * MovieHeaders[28] = {

    "color",
    "director_name",
    "num_critic_for_reviews",
    "duration",
    "director_facebook_likes",
    "actor_3_facebook_likes",
    "actor_2_name",
    "actor_1_facebook_likes",
    "gross",
    "genres",
    "actor_1_name",
    "movie_title",
    "num_voted_users",
    "cast_total_facebook_likes",
    "actor_3_name",
    "facenumber_in_poster",
    "plot_keywords",
    "movie_imdb_link",
    "num_user_for_reviews",
    "language",
    "country",
    "content_rating",
    "budget",
    "title_year",
    "actor_2_facebook_likes",
    "imdb_score",
    "aspect_ratio",
    "movie_facebook_likes"
};

// Removes trailing whitespaces from <str>.
void removeTrail(char * str) {
    
    unsigned long i = strlen(str);
    i--;
    
    while (str[i] == ' ' || str[i] == '\n' || str[i] == '\t' || str[i] == '\r' || str[i] == '\v'
           || str[i] == '\f') {
        i--;
    }
    
    str[i+1] = '\0';
}

// Tokenizes a CSV row into the pre-allocated <row>. <line> is the
// pre-allocated CSV line/row to tokenize. Returns 1 if 27 cells
// have been tokenized, otherwise returns 0. If <isHeader> is non-zero,
// then returns 1 if the headers match the movie_metadata headers.
int tokenizeRow(char * line, char ** row, int isHeader) {
    
    int rc = 0;
    char * tempCell = line;
    char * endLine = line + strlen(line);
    
    int inQuote = 0;
    int beginCell = 1;
    
    while(line < endLine) {
        
        if(*line == ',' && !inQuote && !beginCell) {
                
            *line = '\0';
            row[rc] = tempCell;
            removeTrail(tempCell);
            if (isHeader && strcmp(tempCell, MovieHeaders[rc])) {
                return 0;
            }
            rc++;
            beginCell = 1;
            
        } else if (*line == '"') {
            
            if (inQuote) {
                
                *line = '\0';
                row[rc] = tempCell;
                removeTrail(tempCell);
                if (isHeader && strcmp(tempCell, MovieHeaders[rc])) {
                    return 0;
                }
                rc++;
                beginCell = 1;
                
            } else {
                
                inQuote = 1;
                beginCell = 1;
            }
            
        } else if (beginCell && *line != ' ') {
            
            if (*line == ',') {
                
                if (inQuote) {
                    inQuote = 0;
                    
                } else {
                    
                    *line = '\0';
                    row[rc] = line;
                    if (isHeader && strcmp(line, MovieHeaders[rc])) {
                        return 0;
                    }
                    rc++;
                }
                
            } else {
                
                tempCell = line;
                beginCell = 0;
            }
        }
        
        line++;
    }
    
    row[rc] = tempCell;
    removeTrail(tempCell);
    if (isHeader && strcmp(tempCell, MovieHeaders[rc])) {
        return 0;
    }
    
    if (rc == 27) {
        return 1;
    }
    
    return 0;
}

// Frees <table>
void freeTable(struct table * table) {
    
    free(table->table);
    
    for (unsigned int i = 0; i < table->numRowsMems; i++) {
        free(table->rowsMems[i]);
    }
    
    free(table->rowsMems);
    
    for (unsigned int i = 0; i < table->numCellsMems; i++) {
        free(table->cellsMems[i]);
    }
    
    free(table->cellsMems);
    
    free(table);
}

// Fills newly-allocated *<table> with data from <csvPath>. *<cells> is a
// newly-allocated memory that will be stored with strings of the cells.
// Returns the number of successful rows in table, returns 0 if <csvFile>
// is not a proper movie_metadata CSV. Reallocates <table> and <cells>.
// To free, free <table>[i], 0 < i <return value.
int fillTable(FILE * csv, uint32_t csvSize, struct table * table) {
    
    table->numRowsMems = 1;
    table->numCellsMems = 1;
    table->rowsMems = malloc(sizeof(char **));
    table->cellsMems = malloc(sizeof(char *));
    
    table->table = (char ***) malloc(sizeof(char **) * TEMPSIZE * TEMPSIZE);
    (*(table->rowsMems)) = (char **) malloc(sizeof(char *) * COLUMNS * TEMPSIZE * TEMPSIZE);
    (*(table->cellsMems)) = (char *) malloc(COLUMNS * TEMPSIZE * TEMPSIZE);
    
    unsigned int numRows = 0;
    char * i = (*(table->cellsMems));
    char ** j = (*(table->rowsMems));

    
    while(fgets(i, TEMPSIZE, csv) != NULL && csvSize > 0) {
        
        csvSize -= strlen(i);
        
        table->table[numRows] = j;
        
        unsigned long next = strlen(i) + 1;
        
        int goodRow = 0;
        
        if (!numRows) {
            goodRow = tokenizeRow(i, table->table[numRows], 1);
        } else {
            goodRow = tokenizeRow(i, table->table[numRows], 0);
        }
        
        if (goodRow) {
            
            numRows++;
            i += next;
            j += sizeof(char *) * COLUMNS;
            
        } else if (!numRows) {
            
            fclose(csv);
            freeTable(table);
            return 0;
            
        }
    }
    
    fclose(csv);
    
    table->numRows = numRows;
    
    return 1;
}

// Prints <table> with <rows> rowscolumns in a csv
// (comma seperated values) format to <stream>.
void printTable (FILE * stream, char *** table, unsigned int rows) {
    
    for (int i = 0; i < rows; i++) {
        
        if(strchr(table[i][0], ',') != NULL) {
            fprintf(stream, "\"%s\"", table[i][0]);
            
        } else {
            fprintf(stream, "%s", table[i][0]);
        }
        
        for (int j = 1; j < COLUMNS; j++) {
            
            if(strchr(table[i][j], ',') != NULL) {
                fprintf(stream, ",\"%s\"", table[i][j]);
                
            } else {
                fprintf(stream, ",%s", table[i][j]);
            }
        }
        
        fprintf(stream, "\n");
    }
}

//// If <areNumbers> is set to 0, returns 0 if <y> is lexicographically before <x>,
//// else returns 1. If <areNumbers> is set to anything besides 0, converts <x> and
//// <y> to double values x y respectively, and returns x <= y.
//int isXBeforeY (const char * x, const char * y) {
//    
//    if (IsNumeric) {
//        return atof(x) <= atof(y);
//        
//    } else {
//        return (strcmp(x, y) <= 0);
//    }
//}

// Returns 1 if <csvPath> is a path to a "proper" .csv file, else returns 0.
int isCsv(const char * csvPath) {
    
    const char * extension = csvPath + strlen(csvPath) - 4;
    
    if (!strcmp(".csv", extension)) {
        return 1;
    }
    
    return 0;
}

//// If the name of the CSV file located at <path> is A. This function returns
//// the newly allocated string: "<outputDir>/AllFiles-sorted-<Header>.csv".
//void printToSortedCsv(struct table * table) {
//
//    char sortedCsvPath[strlen(OutputDir) + strlen(Header) + 22];
//    sprintf(sortedCsvPath, "%s/AllFiles-sorted-%s.csv", OutputDir, Header);
//
//    FILE * out = fopen(sortedCsvPath, "w");
//    printTable(out, table->table, table->numRows);
//    fclose(out);
//}

// Checks weather <path> is a valid directory, if not,
// the program crashes with an approiate error message.
void checkDir(const char * path, const char * dirType) {

    DIR * dir = opendir(path);
    
    if (dir == NULL) {
        
        errno = 0;
        
        switch (errno) {
                
            case EACCES:
                fprintf(stderr, "You do not have access to the specified %s directory, %s\n", dirType, path);
                break;
                
            case ENOENT:
                fprintf(stderr, "The specified %s directory, %s, does not exist\n", dirType, path);
                break;
                
            case ENOTDIR:
                fprintf(stderr, "The specified %s directory, %s, is not a directory\n", dirType, path);
                break;
                
            default:
                fprintf(stderr, "A problem occured opening the specified %s directory, %s\n", dirType, path);
                break;
        }
        
        closedir(dir);
        exit(EXIT_FAILURE);
    }
    
    closedir(dir);
}

//// Returns the index of <header>, and sets <IsNumeric>.
//unsigned int getIndex(const char * header) {
//    
//    if (!strcmp("color", header)) {
//        IsNumeric = 0;
//        return 0;
//    } else if (!strcmp("director_name", header)) {
//        IsNumeric = 0;
//        return 1;
//    } else if (!strcmp("num_critic_for_reviews", header)) {
//        IsNumeric = 1;
//        return 2;
//    } else if (!strcmp("duration", header)) {
//        IsNumeric = 1;
//        return 3;
//    } else if (!strcmp("director_facebook_likes", header)) {
//        IsNumeric = 1;
//        return 4;
//    } else if (!strcmp("actor_3_facebook_likes", header)) {
//        IsNumeric = 1;
//        return 5;
//    } else if (!strcmp("actor_2_name", header)) {
//        IsNumeric = 0;
//        return 6;
//    } else if (!strcmp("actor_1_facebook_likes", header)) {
//        IsNumeric = 1;
//        return 7;
//    } else if (!strcmp("gross", header)) {
//        IsNumeric = 1;
//        return 8;
//    } else if (!strcmp("genres", header)) {
//        IsNumeric = 0;
//        return 9;
//    } else if (!strcmp("actor_1_name", header)) {
//        IsNumeric = 0;
//        return 10;
//    } else if (!strcmp("movie_title", header)) {
//        IsNumeric = 0;
//        return 11;
//    } else if (!strcmp("num_voted_users", header)) {
//        IsNumeric = 1;
//        return 12;
//    } else if (!strcmp("cast_total_facebook_likes", header)) {
//        IsNumeric = 1;
//        return 13;
//    } else if (!strcmp("actor_3_name", header)) {
//        IsNumeric = 0;
//        return 14;
//    } else if (!strcmp("facenumber_in_poster", header)) {
//        IsNumeric = 1;
//        return 15;
//    } else if (!strcmp("plot_keywords", header)) {
//        IsNumeric = 0;
//        return 16;
//    } else if (!strcmp("movie_imdb_link", header)) {
//        IsNumeric = 0;
//        return 17;
//    } else if (!strcmp("num_user_for_reviews", header)) {
//        IsNumeric = 1;
//        return 18;
//    } else if (!strcmp("language", header)) {
//        IsNumeric = 0;
//        return 19;
//    } else if (!strcmp("country", header)) {
//        IsNumeric = 0;
//        return 20;
//    } else if (!strcmp("content_rating", header)) {
//        IsNumeric = 0;
//        return 21;
//    } else if (!strcmp("budget", header)) {
//        IsNumeric = 1;
//        return 22;
//    } else if (!strcmp("title_year", header)) {
//        IsNumeric = 1;
//        return 23;
//    } else if (!strcmp("actor_2_facebook_likes", header)) {
//        IsNumeric = 1;
//        return 24;
//    } else if (!strcmp("imdb_score", header)) {
//        IsNumeric = 1;
//        return 25;
//    } else if (!strcmp("aspect_ratio", header)) {
//        IsNumeric = 1;
//        return 26;
//    } else if (!strcmp("movie_facebook_likes", header)) {
//        IsNumeric = 1;
//        return 27;
//    } else {
//        fprintf(stderr, "The specified header, %s, was not found\n", header);
//        exit(EXIT_FAILURE);
//    }
//}

//// Process directory at <param>. Processes subdirectories
//// and sorts files with new threads.
//void * processCsvDir(void * param) {
//
//    char * path = param;
//
//    DIR * dir = opendir(path);
//
//    pthread_t children[TEMPSIZE];
//    unsigned int cc = 0;
//
//    for (struct dirent * entry = readdir(dir); entry != NULL; entry = readdir(dir)) {
//
//        if ((entry->d_type == DT_DIR && strcmp(entry->d_name, ".")
//             && strcmp(entry->d_name, "..")) || entry->d_type == DT_REG) {
//
//            char * subPath = malloc(strlen(path) + strlen(entry->d_name) + 2);
//            sprintf(subPath, "%s/%s", path, entry->d_name);
//
//            if (entry->d_type == DT_DIR) {
//                pthread_create(children + cc, NULL, processCsvDir, subPath);
//
//            } else {
//
//                incrementCsvCount();
//                pthread_create(children + cc, NULL, sortCsv, subPath);
//            }
//
//            printf("%lu,", (unsigned long) children[cc]);
//
//            cc++;
//        }
//    }
//
//    pthread_mutex_lock(&ATCM);
//    AllThreadsCount += cc;
//    pthread_mutex_unlock(&ATCM);
//
//    closedir(dir);
//    free(param);
//    pthread_exit(NULL);
//}

