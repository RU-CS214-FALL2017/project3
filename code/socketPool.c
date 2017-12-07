#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <fcntl.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "clientTools.h"
#include "syncWraps.h"

struct SocketQueueNode {
    
    FILE * socket;
    struct SocketQueueNode * next;
};

struct SocketQueueNode * Head;
struct SocketQueueNode * Tail;
pthread_mutex_t SocketQueueLock = PTHREAD_MUTEX_INITIALIZER;
sem_t SocketQueueSemaphore;
unsigned int TotalSockets;

// Reterns the first socket in the queue and
// removes it. Thread safe.
FILE * popSocket() {
    
    mutexLock(&SocketQueueLock, "SocketQueueLock");
    semWait(&SocketQueueSemaphore, "SocketQueueSemaphore");
    
    struct SocketQueueNode * oldHead = Head;
    Head = oldHead->next;
    
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
        Head = newNode;
    } else {
        Tail->next = newNode;
    }
    Tail = newNode;
    
    semPost(&SocketQueueSemaphore, "SocketQueueSemaphore");
    mutexUnlock(&SocketQueueLock, "SocketQueueLock");
}

// Initializes a pool of <num> sockets that are
// connected to <hostname> on <port>.
void initializeSockets(const char * hostname, const char * port, unsigned int num) {
    
    semInit(&SocketQueueSemaphore, "SocketQueueSemaphore", 0);
    TotalSockets = num;
    
    for (int i = 0; i < num; i++) {
        
        FILE * server = connectToServer(hostname, port);
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
    
    for (int i = 0; i < TotalSockets; i++) {
        
        printf("close loop: %d\n", i);
        FILE * server = popSocket();
        fwrite("done", 4, 1, server);
        fclose(server);
    }
}
