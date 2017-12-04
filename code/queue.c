#include <pthread.h>
#include <stdlib.h>

#include "queue.h"

pthread_cond_t QCV = PTHREAD_COND_INITIALIZER;
pthread_mutex_t QM = PTHREAD_MUTEX_INITIALIZER;

struct node * head;
struct node * tail;

unsigned int QElements = 0;

// Reterns the first TID in the queue and
// removes it.
pthread_t popTid() {
    
    QElements--;
    
    struct node * oldHead = head;
    head = oldHead->next;
    
    pthread_t ret = oldHead->tid;
    free(oldHead);
    
    return ret;
}

// Inserts <tid> at the end of the queue.
// Thread safe.
void pushTid(pthread_t tid) {
    
    struct node * newNode = (struct node *) malloc(sizeof(struct node));
    newNode->tid = tid;

    pthread_mutex_lock(&QM);

    if (QElements == 0) {
        head = newNode;
        
    } else {
        tail->next = newNode;
    }
    
    tail = newNode;
    
    QElements++;
    
    pthread_cond_signal(&QCV);
    pthread_mutex_unlock(&QM);
}
