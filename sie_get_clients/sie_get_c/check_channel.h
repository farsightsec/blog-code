#define SMALL    20
#define LARGE    255

/* prototypes */
void check_channel(void);
void check_intervals(void);
long string_fmt_time_to_seconds(const char *);

/* global variables */
extern char firstarg[];
extern char secarg[];
extern char thirdarg[];

extern char earliestdate_avail[SMALL];
extern char latestdate_avail[SMALL];
extern char volume[SMALL];
