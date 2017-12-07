#ifndef sorter_h
#define sorter_h

int sortByHeader(const char * header, struct Table * table);
struct Table * mergeTables(struct Table * table1, struct Table * table2, unsigned int sortIndex, int isNumeric);

#endif /* sorter_h */
