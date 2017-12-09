#include <pthread.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include "tools.h"
#include "dirTools.h"
#include "clientTools.h"

struct Params {
    
    char * path;
    uint32_t id;
};

// Thread helper function.
void * processCsvDirHelper(void * parameters) {
    printf("in dir helper\n");fflush(stdout);
    struct Params * params = parameters;printf("subPath: %s\n", params->path);
    processCsvDir(params->path, params->id);
    free(params->path);
    free(parameters);
    
    pthread_exit(NULL);
}

void * sortCsvHelper(void * parameters) {
    printf("in csv helper\n");fflush(stdout);
    struct Params * params = parameters;printf("subPath: %s\n", params->path);
    sortCsv(params->path, params->id);
    free(params->path);
    free(parameters);
    
    pthread_exit(NULL);
}

// Process directory at <param>. Processes subdirectories
// and sorts files with new threads.
void processCsvDir(const char * path, uint32_t id) {
    printf("path: %s\n", path);
    DIR * dir = opendir(path);
    
    pthread_t children[TEMPSIZE];
    unsigned int cc = 0;
    
    for (struct dirent * entry = readdir(dir); entry != NULL; entry = readdir(dir)) {
        
        if ((entry->d_type == DT_DIR && strcmp(entry->d_name, ".")
             && strcmp(entry->d_name, "..")) || entry->d_type == DT_REG) {
            
            struct Params * params = malloc(sizeof(struct Params));
            params->id = id;if (params == NULL) {printf("NULL params\n");}
            params->path = malloc(strlen(path) + strlen(entry->d_name) + 2);if (params->path == NULL) {printf("NULL path\n");}
            sprintf(params->path, "%s/%s", path, entry->d_name);
            
            if (entry->d_type == DT_DIR) {printf("make childern %s\n", params->path);
                int some = pthread_create(children + cc, NULL, processCsvDirHelper, (void *) params);
                printf("errvaldir: %d\n", some);
            } else {printf("make sort %s\n", params->path);
                int some = pthread_create(children + cc, NULL, sortCsvHelper, (void *) params);printf("errvalcsv: %d\n", some);
            }

            cc++;
        }
    }
    
    closedir(dir);
    
    printf("endcc: %u\n", cc);fflush(stdout);
    for (unsigned int i = 0; i < cc; i++) {printf("gg: %u\n", cc);fflush(stdout);
        pthread_join(children[i], NULL);
//        printf("error: %d\n", pthread_join(children[cc], NULL));
    printf("enjjdcc: %u\n", cc);fflush(stdout);}
}


// Checks weather <path> is a valid directory, if not,
// the program crashes with an approiate error message.
void checkDir(const char * path, const char * dirType) {
    
    DIR * dir = opendir(path);
    
    if (dir == NULL) {
        
        errno = 0;
        
        switch (errno) {
                
            case EACCES:
                printf("You do not have access to the specified %s directory, %s\n", dirType, path);
                break;
                
            case ENOENT:
                printf("The specified %s directory, %s, does not exist\n", dirType, path);
                break;
                
            case ENOTDIR:
                printf("The specified %s directory, %s, is not a directory\n", dirType, path);
                break;
                
            default:
                printf("A problem occured opening the specified %s directory, %s", dirType, path);
                break;
        }
        
        closedir(dir);
        exit(EXIT_FAILURE);
    }
    
    closedir(dir);
}

