#include <pthread.h>
#include <stdlib.h>
#include "syncWraps.h"

#include "tools.h"
#include "sorter.h"

pthread_mutex_t ListMutex = PTHREAD_MUTEX_INITIALIZER;

struct CsvsNode {
    
//    pthread_mutex_t lock;
    uint32_t id;
    unsigned int sortIndex;
    int isNumericColumn;
    struct Table * table;
    struct CsvsNode * next;
    
} * Head = NULL;

void initializeId(uint32_t id, unsigned int sortIndex) {
    
    struct CsvsNode * newNode = malloc(sizeof(struct CsvsNode));
    mutexInit(&(newNode->lock), "CsvsNodeLock");
    newNode->id = id;
    newNode->sortIndex = sortIndex;
    newNode->table = NULL;
    newNode->next = NULL;
    
    mutexLock(&ListMutex, "CsvsListLock");
    
    newNode->next = Head;
    Head = newNode;
    
    mutexUnlock(&ListMutex, "CsvsListLock");
}

int addTable(struct Table * table, uint32_t id) {
    
    mutexLock(&ListMutex, "CsvsListLock");
    
    for (struct CsvsNode * i = Head; i != NULL; i = Head->next) {
        
        if (i->id == id) {
            
//            mutexLock(&(i->lock), "TableLock");
            
            if (i->table == NULL) {
                i->table = table;
                
            } else if (sameHeaders(i->table, table)) {
                i->table = mergeTables(i->table, table, i->sortIndex, i->isNumericColumn);
                
            } else {
                
                fprintf(stderr, "Table not compatible.\n");
//                mutexUnlock(&(i->lock), "TableLock");
                return 0;
            }
            
//            mutexUnlock(&(i->lock), "TableLock");
            return 1;
        }
    }
    
    mutexUnlock(&ListMutex, "CsvsListLock");
    
    fprintf(stderr, "ID %u not found\n", id);
    return 0;
}

struct Table * dumpTable(uint32_t id) {
    
    mutexLock(&ListMutex, "CsvsListLock");
    
    struct CsvsNode * temp = Head;
    
    for (struct CsvsNode * i = Head; i != NULL; i = Head->next) {
        
        if (i->id == id) {
            
            temp->next = i->next;
            struct Table * ret = i->table;
            free(i);
            return ret;
        }
        
        temp = i;
    }
    
    mutexUnlock(&ListMutex, "CsvsListLock");
    
    return NULL;
}
