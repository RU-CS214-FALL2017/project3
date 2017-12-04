#ifndef sorter_h
#define sorter_h

extern int CsvCounter;
extern pthread_mutex_t CCM;
extern pthread_mutex_t QM;

extern int DoneSorting;
extern pthread_cond_t DSCV;
extern pthread_mutex_t DSM;

extern char * CsvErrors;
extern pthread_mutex_t CEM;

extern char * Header;
extern unsigned int SortIndex;
extern int IsNumeric;
extern char * OutputDir;

void incrementCsvCount(void);
void decrementCsvCount(void);
void * sortCsv(void * param);

#endif /* sorter_h */
