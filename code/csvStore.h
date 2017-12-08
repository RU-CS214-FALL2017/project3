#ifndef csvStore_h
#define csvStore_h

void initializeId(uint32_t id, char * columnHeader);
int checkHeaders(struct Table * table, uint32_t id);
int addTable(struct Table * table, uint32_t id);
struct Table * dumpTable(uint32_t id);
int getInfo(uint32_t id, unsigned int * sortIndex, int * isNumeric);

#endif /* csvStore_h */
