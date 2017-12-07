#include <string.h>
#include <stdlib.h>

#include "tools.h"
#include "sorter.h"

// Merges sub rows from <table>, where subrows1 is from row at index <start>
// inclusive to row at index <mid> exclusive, and subrows2 is from row at
// index <mid> inclusive to row at index <end> exclusive. As the sub rows
// merge, they are sorted ascendingly according to the column at index
// <columnIndex>. If areNumbers is set to 0, the sort is done numerically
// otherwise it is done lexicographically.
void merge(char *** table, const unsigned int columnIndex, const int areNumbers,
           const unsigned int start, const unsigned int mid,  const unsigned int end) {
    
    char ** temp[end - start];
    unsigned int s = start;
    unsigned int m = mid;
    unsigned int i = 0;
    
    while(s < mid && m < end) {
        
        if(isXBeforeY((table)[s][columnIndex], (table)[m][columnIndex], areNumbers)) {
            
            temp[i] = (table)[s];
            s++;
            
        } else {
            
            temp[i] = (table)[m];
            m++;
        }
        
        i++;
    }
    
    while (s < mid) {
        
        temp[i] = (table)[s];
        s++;
        i++;
    }
    
    while (m < end) {
        
        temp[i] = (table)[m];
        m++;
        i++;
    }
    
    s = start;
    
    for (int j = 0; j < i; j++) {
        
        (table)[s] = temp[j];
        s++;
    }
}

// Ascendingly sorts <table> according to the column at index <columnIndex> from row at
// index <start> to row at index <end>. If areNumbers is set to 0, the sort
// is done numerically otherwise it is done lexicographically.
void mergeSort(char *** table, const unsigned int columnIndex, const int areNumbers,
               const unsigned int start, const unsigned int end) {
    
    if ((end - start) > 1 ) {
        
        unsigned int mid = ((end - start) / 2) + start;
        
        mergeSort(table, columnIndex, areNumbers, start, mid);
        mergeSort(table, columnIndex, areNumbers, mid, end);
        
        merge(table, columnIndex, areNumbers, start, mid, end);
    }
}

// Ascendingly sorts <table> with <rows> rows and <columns> columns according to
// the column with the header <columnHeader>. Returns 1 if <columnHeader>
// was found, else returns 0.
int sortByHeader(const char * header, struct Table * table) {
    
    unsigned int columnIndex = getColumnHeaderIndex(header, table);
    if (columnIndex == -1) {
        return 0;
    }
    
    mergeSort(table->table, columnIndex, isNumericColumn(table, columnIndex), 1, table->rows);
    return 1;
}

// Merges <table1> and <table2>. The input tables must be pre-sorted.
// Returns a new, sorted, merged table.
struct Table * mergeTables(struct Table * table1, struct Table * table2, unsigned int sortIndex, int isNumeric) {
    
    if (table1->rows == 0) {
        return table2;
    }
    
    if (table2->rows == 0) {
        return table1;
    }
    
    struct Table * table = malloc(sizeof(struct Table));
    table->table = malloc(sizeof(char **) * (table1->rows + table2->rows - 1));
    table->rows = table1->rows + table2->rows - 1;
    table->columns = table1->columns;
    
    unsigned int fc = 1;
    unsigned int sc = 1;
    unsigned int tc = 1;
    
    table->table[0] = table1->table[0];
    
    while (fc < table1->rows && sc < table2->rows) {
        
        if (isXBeforeY((table1->table)[fc][sortIndex], (table2->table)[sc][sortIndex], isNumeric)) {
            
            (table->table)[tc] = (table1->table)[fc];
            fc++;
            
        } else {
            
            (table->table)[tc] = (table2->table)[sc];
            sc++;
        }
        
        tc++;
    }
    
    while (fc < table1->rows) {
        
        (table->table)[tc] = (table1->table)[fc];
        fc++;
        tc++;
    }
    
    while (sc < table2->rows) {
        
        (table->table)[tc] = (table2->table)[sc];
        sc++;
        tc++;
    }
    
    free(table1->table);
    free(table2->table);
    
    return table;
}
