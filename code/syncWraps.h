#ifndef syncWraps_h
#define syncWraps_h

void mutexInit(pthread_mutex_t * m, const char * name);
void mutexLock(pthread_mutex_t * m, const char * name);
void mutexUnlock(pthread_mutex_t * m, const char * name);
void semWait(sem_t * s, const char * name);
int semGetValue(sem_t * s, const char * name);
void semPost(sem_t * s, const char * name);
void semInit(sem_t * s, const char * name, unsigned int num);

#endif /* syncWraps_h */
