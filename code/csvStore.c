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

// Returns code.
char checkHeaders(struct Table * table, uint32_t id) {
    
    mutexLock(&ListMutex, "CsvsListLock");
    
    for (struct CsvsNode * i = Head; i != NULL; i = Head->next) {
        
        if (i->id == id) {
            
            if (i->table == NULL) {
                
                i->sortIndex = getColumnHeaderIndex(i->columnHeader, table);
                
                if (i->sortIndex == -1) {

                    mutexUnlock(&ListMutex, "CsvsListLock");
                    return COLUMN_HEADER_NOT_FOUND;
                }
                
                i->isNumericColumn = isNumericColumn(table, i->sortIndex);
                
            } else if (!sameHeaders(i->table, table)) {
                
                mutexUnlock(&ListMutex, "CsvsListLock");
                return TABLE_INCOMPATIBLE;
            }
            
            mutexUnlock(&ListMutex, "CsvsListLock");
            return SUCCESS;
        }
    }
    
    mutexUnlock(&ListMutex, "CsvsListLock");
    return ID_NOT_FOUND;
}

// Adds a table to the store. Returns 0 if <id> found,
// else returns error code.
char addTable(struct Table * table, uint32_t id) {
    
    mutexLock(&ListMutex, "CsvsListLock");
    
    for (struct CsvsNode * i = Head; i != NULL; i = Head->next) {
        
        if (i->id == id) {
            
            if (i->table == NULL) {
                i->table = table;
                
            } else {
                i->table = mergeTables(i->table, table, i->sortIndex, i->isNumericColumn);
            }
            
            mutexUnlock(&ListMutex, "CsvsListLock");
            return SUCCESS;
        }
    }
    
    return ID_NOT_FOUND;
}

// Dumps a table from the store. Returns NULL if
// <id> not found.
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

// Returns 0 if <id> found, else returns error code.
char getInfo(uint32_t id, unsigned int * sortIndex, int * isNumeric) {
    
    mutexLock(&ListMutex, "CsvsListLock");
    
    for (struct CsvsNode * i = Head; i != NULL; i = Head->next) {
        
        if (i->id == id) {
            
            printf("getinfo sortIndex: %u\n", i->sortIndex);
            printf("getinfo isNumericColumn: %d\n", i->isNumericColumn);
            *sortIndex = i->sortIndex;
            *isNumeric = i->isNumericColumn;
            
            mutexUnlock(&ListMutex, "CsvsListLock");
            return SUCCESS;
        }
    }
    
    mutexUnlock(&ListMutex, "CsvsListLock");
    
    return ID_NOT_FOUND;
}
