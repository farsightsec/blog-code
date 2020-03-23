#define SMALL    20
#define LARGE    200

/* prototypes */
char *getkeyfromlocalfile(void);
char *make_query(char[LARGE], char[LARGE]);
char *replString(const char *, const char *, const char *);
void check_intervals(void);

/* global variables */
extern char firstarg[];
extern char secarg[];
extern char thirdarg[];
extern char endpoint[];
extern char outputfilename[];

/* defined here, so no extern */
char earliestdate_avail[SMALL];
char latestdate_avail[SMALL];
char volume[SMALL];
