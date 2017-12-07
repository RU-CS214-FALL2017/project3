#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <stdlib.h>

void mutexInit(pthread_mutex_t * m, const char * name) {
    
    if (pthread_mutex_init(m, NULL) != 0) {
        
        fprintf(stderr, "Error initializing %s\n", name);
        exit(EXIT_FAILURE);
    }
}

void mutexLock(pthread_mutex_t * m, const char * name) {
    
    if (pthread_mutex_lock(m) != 0) {
        
        fprintf(stderr, "Error locking to %s\n", name);
        exit(EXIT_FAILURE);
    }
}

void mutexUnlock(pthread_mutex_t * m, const char * name) {
    
    if (pthread_mutex_unlock(m) != 0) {
        
        fprintf(stderr, "Error unlocking to %s\n", name);
        exit(EXIT_FAILURE);
    }
}

void semWait(sem_t * s, const char * name) {
    
    if (sem_wait(s) == -1) {
        
        fprintf(stderr, "Error waiting on %s\n", name);
        exit(EXIT_FAILURE);
    }
}

int semGetValue(sem_t * s, const char * name) {
    
    int value;
    
    if (sem_getvalue(s, &value) == -1) {
        
        fprintf(stderr, "Error getting value of %s\n", name);
        exit(EXIT_FAILURE);
    }
    
    return value;
}

void semPost(sem_t * s, const char * name) {
    
    if (sem_post(s) == -1){
        
        fprintf(stderr, "Error posting to %s\n", name);
        exit(EXIT_FAILURE);
    }
}

void semInit(sem_t * s, const char * name, unsigned int num) {
    
    if (sem_init(s, 0, num) == -1) {
        
        fprintf(stderr, "Error initializing to %s\n", name);
        exit(EXIT_FAILURE);
    }
}
