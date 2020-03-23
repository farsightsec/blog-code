#define LARGE    200

/* prototypes */
char *make_query(char[LARGE], char[LARGE]);
void list_channels();
char *getkeyfromlocalfile();
char *replString(const char *, const char *, const char *);

/* global variables */
extern char endpoint[];
extern char outputfilename[];
