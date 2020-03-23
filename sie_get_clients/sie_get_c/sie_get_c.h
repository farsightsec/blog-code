#define SMALL    20

/* global variables */
char endpoint[] = "batch.sie-remote.net";
char useproxy[] = "no"; /* "yes" or "no" */

int  myargcount;
char firstarg[SMALL]; /* NULL, an actual channum, checkkey, channels */
char secarg[SMALL];   /* NULL, "2020-05-25 12:14:42" */
char thirdarg[SMALL]; /* NULL, "2020-05-25 12:14:42" */
