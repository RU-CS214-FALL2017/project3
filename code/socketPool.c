#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
//#include <fcntl.h>
//#include <stdint.h>
//#include <unistd.h>
//#include <sys/types.h>
//#include <sys/socket.h>

#include "clientTools.h"
#include "syncWraps.h"

// Node for the linked list of sockets in the queue.
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
FILE * getSocket() {
    
    semWait(&SocketQueueSemaphore, "SocketQueueSemaphore");
    mutexLock(&SocketQueueLock, "SocketQueueLock");
    
    struct SocketQueueNode * oldHead = Head;
    Head = oldHead->next;
    
    mutexUnlock(&SocketQueueLock, "SocketQueueLock");
    
    FILE * ret = oldHead->socket;
    free(oldHead);
    
    return ret;
}

// Inserts <socket> at the end of the queue.
// Thread safe.
void returnSocket(FILE * socket) {
    
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
    
    mutexUnlock(&SocketQueueLock, "SocketQueueLock");
    semPost(&SocketQueueSemaphore, "SocketQueueSemaphore");
}

// Initializes a pool of <num> sockets that are
// connected to <hostname> on <port>.
void initializeSockets(const char * hostname, const char * port, unsigned int num) {
    
    semInit(&SocketQueueSemaphore, "SocketQueueSemaphore", 0);
    TotalSockets = num;
    
    for (int i = 0; i < num; i++) {
        
        FILE * server = connectToServer(hostname, port);
        returnSocket(server);
    }
}

// Closes all the sockets in the pool, and signals the server
// todo the same.
void closeSockets() {
    
    for (int i = 0; i < TotalSockets; i++) {
        
        FILE * server = getSocket();
        fwrite("done", 4, 1, server);
        fclose(server);
    }
}
