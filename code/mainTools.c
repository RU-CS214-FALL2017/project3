#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Returns the value of the flag at <flagIndex> of <argv> with
// <argc> elements. Returns NULL if the value is not found.
char * getFlagValueHelper(int flagIndex, int argc, char ** argv) {
    
    if ((flagIndex + 1) >= argc) {
        return NULL;
        
    } else {
        return argv[flagIndex + 1];
    }
}

// Returns the index of <flag> in <argv> with <argc> elements.
// Returns -1 if <flag> not found.
int findFlag(const char * flag, int argc, char ** argv) {
    
    for (int i = 0; i < argc; i++) {
        
        if (!strcmp(flag, argv[i])) {
            return i;
        }
    }
    
    return -1;
}

// Returns the value of <flag> in <argv> with <argc> elements.
// Returns NULL if the flag value is not found.
char * getFlagValue(const char * flag, int argc, char ** argv) {
    
    int flagIndex = findFlag(flag, argc, argv);
    
    if (flagIndex == -1) {
        return NULL;
        
    } else {
        return getFlagValueHelper(flagIndex, argc, argv);
    }
}

// Returns the port to bind on. If <argv> doesn't
// have a specified port, the function prints an
// error and exits with a failure of status.
char * getPort(int argc, char ** argv) {
    
    char * port = getFlagValue("-p", argc, argv);
    
    if (port == NULL) {
        
        fprintf(stderr, "Port not specified\n");
        exit(EXIT_FAILURE);
        
    } else {
        return port;
    }
}

// Returns the column header to sort by. If <argv> doesn't
// have a specified column header, the functions prints an
// error and exits with a failure of status.
char * getColumnHeader(int argc, char ** argv) {
    
    char * columnHeader = getFlagValue("-c", argc, argv);
    
    if (columnHeader == NULL) {
        
        fprintf(stderr, "Column header not specified\n");
        exit(EXIT_FAILURE);
        
    } else {
        return columnHeader;
    }
}

// Returns the hostname. If <argv> doesn't have the hostname,
// this function prints and error and exits.
char * getHostname(int argc, char ** argv) {
    
    char * hostname = getFlagValue("-h", argc, argv);
    
    if (hostname == NULL) {
        
        fprintf(stderr, "Hostname not specified\n");
        exit(EXIT_FAILURE);
        
    } else {
        return hostname;
    }
}

// Returns the input directory if found in <argv>, else returns NULL.
char * getInputDirectory(int argc, char ** argv) {
    return getFlagValue("-d", argc, argv);
}

// Returns the output directory if found in <argv>, else returns NULL.
char * getOutputDirectory(int argc, char ** argv) {
    return getFlagValue("-o", argc, argv);
}

// Returns the pool size if found in <argv>, else returns NULL.
char * getPoolSize(int argc, char ** argv) {
    return getFlagValue("-s", argc, argv);
}
