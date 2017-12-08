#ifndef csvStore_h
#define csvStore_h

void initializeId(uint32_t id, char * columnHeader);
char checkHeaders(struct Table * table, uint32_t id);
char addTable(struct Table * table, uint32_t id);
struct Table * dumpTable(uint32_t id);
char getInfo(uint32_t id, unsigned int * sortIndex, int * isNumeric);

#endif /* csvStore_h */
