#define SMALL    20
#define LARGE    200

/* prototypes */
void build_filename(void);
void check_channel(void);
void fix_times(void);
char *getkeyfromlocalfile(void);
char *make_query(char[LARGE], char[LARGE]);
void three_real_args(void);

/* global variables */

extern char endpoint[];
extern char firstarg[];
extern char secarg[];
extern char thirdarg[];
extern char outputfilename[];
