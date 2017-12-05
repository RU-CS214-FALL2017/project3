#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <stdint.h>

#include "clientTools.h"
#include "syncWraps.h"

struct SocketQueueNode {
    
    FILE * socket;
    struct SocketQueueNode * next;
};

struct SocketQueueNode * head;
struct SocketQueueNode * tail;
pthread_mutex_t SocketQueueLock = PTHREAD_MUTEX_INITIALIZER;
sem_t SocketQueueSemaphore;

// Reterns the first socket in the queue and
// removes it. Thread safe.
FILE * popSocket() {
    
    mutexLock(&SocketQueueLock, "SocketQueueLock");
    semWait(&SocketQueueSemaphore, "SocketQueueSemaphore");
    
    struct SocketQueueNode * oldHead = head;
    head = oldHead->next;
    
    mutexUnlock(&SocketQueueLock, "SocketQueueLock");
    
    FILE * ret = oldHead->socket;
    free(oldHead);
    
    return ret;
}

// Inserts <socket> at the end of the queue.
// Thread safe.
void pushSocket(FILE * socket) {
    
    struct SocketQueueNode * newNode = malloc(sizeof(struct SocketQueueNode));
    newNode->socket = socket;
    
    mutexLock(&SocketQueueLock, "SocketQueueLock");
    
    int elements = semGetValue(&SocketQueueSemaphore, "SocketQueueSemaphore");
    
    if (elements == 0) {
        head = newNode;
    } else {
        tail->next = newNode;
    }
    tail = newNode;
    
    semPost(&SocketQueueSemaphore, "SocketQueueSemaphore");
    mutexUnlock(&SocketQueueLock, "SocketQueueLock");
}

// Initializes a pool of <num> sockets that are
// connected to <hostname> on <port>.
void initializeSockets(const char * hostname, const char * port, unsigned int num) {
    
    semInit(&SocketQueueSemaphore, "SocketQueueSemaphore", num);
    
    for (int i = 0; i < num; i++) {
        
        FILE * server = connectToServer(hostname, port);
        fprintf(server, "none");
        pushSocket(server);
    }
}

// Returns a socket from the pool.
FILE * getSocket() {
    return popSocket();
}

// Place <socket> back into the pool.
void returnSocket(FILE * socket) {
    pushSocket(socket);
}

// Closes all the sockets in the pool.
void closeSockets() {
    
    mutexLock(&SocketQueueLock, "SocketQueueLock");
    
    int elements = semGetValue(&SocketQueueSemaphore, "SocketQueueSemaphore");
    
    while (elements > 0) {
        
        fclose(popSocket());
        elements = semGetValue(&SocketQueueSemaphore, "SocketQueueSemaphore");
    }
    
    mutexUnlock(&SocketQueueLock, "SocketQueueLock");
}
