#include <inttypes.h>

#include "mainTools.h"
#include "clientTools.h"
#include "socketPool.h"
#include "dirTools.h"

int main(int argc, char ** argv) {
    
    char * hostname = getHostname(argc, argv);
    char * port = getPort(argc, argv);
    char * header = getColumnHeader(argc, argv);
    char * inputDir = getInputDirectory(argc, argv);
    char * outputDir = getOutputDirectory(argc, argv);
    unsigned int poolSize = getPoolSize(argc, argv);
    
    checkDir(inputDir, "input");
    checkDir(outputDir, "output");
    
    initializeSockets(hostname, port, poolSize);
    
    uint32_t id = requestId(header);
    processCsvDir(inputDir, id);
    retrieveAndSaveCsv(id, outputDir, header);
    
    closeSockets();
}

