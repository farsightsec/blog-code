#define SMALL    20
#define LARGE    200

/* prototypes */
char *validateapikeyonline(int, char *, char *);
char *getkeyfromlocalfile(void);
char *make_query(char[LARGE], char[LARGE]);

char *result[SMALL];

/* global variables */
extern char endpoint[];
extern char outputfilename[LARGE];
