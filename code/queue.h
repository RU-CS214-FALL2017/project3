#ifndef queue_h
#define queue_h

#include "tools.h"

extern pthread_cond_t QCV;
extern pthread_mutex_t QM;
extern unsigned int QElements;

// Represents queue's linked-list's node.
struct node {
  
    pthread_t tid;
    struct node * next;
};

pthread_t popTid(void);
void pushTid(pthread_t tid);

#endif /* queue_h */
