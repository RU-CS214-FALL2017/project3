#include <pthread.h>
#include <stdlib.h>
#include "syncWraps.h"

#include "tools.h"

pthread_mutex_t ListMutex = PTHREAD_MUTEX_INITIALIZER;

struct CsvsNode {
    
    pthread_mutex_t lock;
    uint32_t id;
    unsigned int sortIndex;
    int isNumericColumn;
    struct Table table;
    struct CsvsNode * next;
    
} * Head = NULL;

void initializeId(uint32_t id, unsigned int sortIndex) {
    
    struct CsvsNode * newNode = malloc(sizeof(struct CsvsNode));
    mutexInit(&(newNode->lock), "CsvsNodeLock");
    newNode->id = id;
    newNode->sortIndex = sortIndex;
    newNode->table.rows = 0;
    newNode->table.columns = 0;
    newNode->next = NULL;
    
    mutexLock(&ListMutex, "CsvsListLock");
    
    newNode->next = Head;
    Head = newNode;
}

void addTable(struct Table * table, uint32_t id) {
    
    
}
