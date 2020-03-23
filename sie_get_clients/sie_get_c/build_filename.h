#define SMALL    20
#define LARGE    200

/* prototypes */

int search_a_list(const char *, const char **const);
void build_filename(void);
char *replString(const char *, const char *, const char *);

/* global variables */

extern char firstarg[];
extern char secarg[];
extern char thirdarg[];

/* created here, no extern */
char outputfilename[LARGE];
