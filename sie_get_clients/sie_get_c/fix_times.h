#define SMALL    20
#define BIG      200

/* prototypes */
void convert_relative_times_to_real_datetimes(void);
void fix_times(void);
void validate_input_time_date_format(char[SMALL]);

/* global variables */
extern char secarg[];
extern char thirdarg[];
