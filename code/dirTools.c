#include <pthread.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
//#include <unistd.h>
//
#include "tools.h"
#include "dirTools.h"
#include "clientTools.h"

// Pthread params for the two helper processCsvDir
// and sortCsv.
struct Params {
    
    char * path;
    uint32_t id;
};

// Pthread helper function for processCsvDir.
void * processCsvDirHelper(void * parameters) {

    struct Params * params = parameters;
    processCsvDir(params->path, params->id);
    free(params->path);
    free(parameters);
    
    pthread_exit(NULL);
}

// Pthread helper function for sortCsv.
void * sortCsvHelper(void * parameters) {

    struct Params * params = parameters;
    sortCsv(params->path, params->id);
    free(params->path);
    free(parameters);
    
    pthread_exit(NULL);
}

// Searches directory at <path>. Recursive on all subdirectories.
// Sorts all CSV files using <id>. Multithreaded.
void processCsvDir(const char * path, uint32_t id) {

    DIR * dir = opendir(path);
    
    pthread_t children[TEMPSIZE];
    unsigned int cc = 0;
    
    for (struct dirent * entry = readdir(dir); entry != NULL; entry = readdir(dir)) {
        
        if ((entry->d_type == DT_DIR && strcmp(entry->d_name, ".")
             && strcmp(entry->d_name, "..")) || entry->d_type == DT_REG) {
            
            struct Params * params = malloc(sizeof(struct Params));
            params->id = id;
            params->path = malloc(strlen(path) + strlen(entry->d_name) + 2);
            sprintf(params->path, "%s/%s", path, entry->d_name);
            
            if (entry->d_type == DT_DIR) {
                pthread_create(children + cc, NULL, processCsvDirHelper, params);
             
            } else {
                pthread_create(children + cc, NULL, sortCsvHelper, params);
            }

            cc++;
        }
    }
    
    closedir(dir);
    
    for (unsigned int i = 0; i < cc; i++) {
        pthread_join(children[i], NULL);
    }
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
