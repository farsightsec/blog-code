extern int  myargcount;
extern char firstarg[];
extern char secarg[];
extern char thirdarg[];

/* prototypes */
void list_channels(void);
void one_real_arg(void);
void print_usage_info(void);
void format_and_printout_chan_time_limits(void);
void printout_intervals(void);
char *validateapikeyonline();
int isNumeric(char *);
int search_a_list(const char *, const char **const);
