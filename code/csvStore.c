#include <pthread.h>
#include <stdlib.h>
#include "syncWraps.h"

#include "tools.h"
#include "sorter.h"

pthread_mutex_t ListMutex = PTHREAD_MUTEX_INITIALIZER;

struct CsvsNode {
    
//    pthread_mutex_t lock;
    uint32_t id;
    char * columnHeader;
    unsigned int sortIndex;
    int isNumericColumn;
    struct Table * table;
    struct CsvsNode * next;
    
} * Head = NULL;

// Initializes an ID in the store.
void initializeId(uint32_t id, char * columnHeader) {
    
    struct CsvsNode * newNode = malloc(sizeof(struct CsvsNode));
//    mutexInit(&(newNode->lock), "CsvsNodeLock");
    newNode->id = id;
    newNode->columnHeader = columnHeader;
    newNode->isNumericColumn = -1;
    newNode->table = NULL;
    newNode->next = NULL;
    
    mutexLock(&ListMutex, "CsvsListLock");
    
    newNode->next = Head;
    Head = newNode;
    
    mutexUnlock(&ListMutex, "CsvsListLock");
}

int checkHeaders(struct Table * table, uint32_t id) {
    
    mutexLock(&ListMutex, "CsvsListLock");
    
    for (struct CsvsNode * i = Head; i != NULL; i = Head->next) {
        
        if (i->id == id) {
            
            //            mutexLock(&(i->lock), "TableLock");
            
            if (i->table == NULL) {
                
                i->sortIndex = getColumnHeaderIndex(i->columnHeader, table);
                if (i->sortIndex == -1) {
                    fprintf(stderr, "Column header %s not found in CSV\n", i->columnHeader);
                    mutexUnlock(&ListMutex, "CsvsListLock");
                    return 0;
                }
                i->isNumericColumn = isNumericColumn(table, i->sortIndex);
                
            } else if (!sameHeaders(i->table, table)) {
                
                fprintf(stderr, "Table not compatible.\n");
                mutexUnlock(&ListMutex, "CsvsListLock");
                return 0;
            }
            
            //            mutexUnlock(&(i->lock), "TableLock");
            mutexUnlock(&ListMutex, "CsvsListLock");
            return 1;
        }
    }
    
    mutexUnlock(&ListMutex, "CsvsListLock");
    
    fprintf(stderr, "ID %u not found\n", id);
    return 0;
}

// Adds a table to the store.
int addTable(struct Table * table, uint32_t id) {
    
    mutexLock(&ListMutex, "CsvsListLock");
    
    for (struct CsvsNode * i = Head; i != NULL; i = Head->next) {
        
        if (i->id == id) {
            
//            mutexLock(&(i->lock), "TableLock");
            
            if (i->table == NULL) {
                i->table = table;
                
            } else {
                i->table = mergeTables(i->table, table, i->sortIndex, i->isNumericColumn);
            }
            
//            mutexUnlock(&(i->lock), "TableLock");
            mutexUnlock(&ListMutex, "CsvsListLock");
            return 1;
        }
    }
    
    mutexUnlock(&ListMutex, "CsvsListLock");
    
    fprintf(stderr, "ID %u not found\n", id);
    return 0;
}

// Dumps a table from the store.
struct Table * dumpTable(uint32_t id) {
    
    mutexLock(&ListMutex, "CsvsListLock");
    
    struct CsvsNode * temp = Head;
    
    for (struct CsvsNode * i = Head; i != NULL; i = Head->next) {
        
        if (i->id == id) {
            
            temp->next = i->next;
            struct Table * ret = i->table;
            free(i);
            mutexUnlock(&ListMutex, "CsvsListLock");
            return ret;
        }
        
        temp = i;
    }
    
    mutexUnlock(&ListMutex, "CsvsListLock");
    
    return NULL;
}

// Returns 1 if <id> found, else returns 0.
int getInfo(uint32_t id, unsigned int * sortIndex, int * isNumeric) {
    
    mutexLock(&ListMutex, "CsvsListLock");
    
    for (struct CsvsNode * i = Head; i != NULL; i = Head->next) {
        
        if (i->id == id) {
            
            printf("getinfo sortIndex: %u\n", i->sortIndex);
            printf("getinfo isNumericColumn: %d\n", i->isNumericColumn);
            *sortIndex = i->sortIndex;
            *isNumeric = i->isNumericColumn;
            
            mutexUnlock(&ListMutex, "CsvsListLock");
            return 1;
        }
    }
    
    mutexUnlock(&ListMutex, "CsvsListLock");
    
    return 0;
}
