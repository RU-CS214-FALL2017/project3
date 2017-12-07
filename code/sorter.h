#ifndef sorter_h
#define sorter_h

int sortById(uint32_t id, struct Table * table);
struct Table * mergeTables(struct Table * table1, struct Table * table2, unsigned int sortIndex, int isNumeric);

#endif /* sorter_h */
