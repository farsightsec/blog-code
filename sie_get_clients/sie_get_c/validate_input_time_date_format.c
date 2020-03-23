#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <regex.h>
#include "validate_input_time_date_format.h"

void validate_input_time_date_format(char *mydatetime)
{
   /* make sure the user has followed the required datetime format */
   /* parameter is datetime to format check. if invalid, abort run. */
   /* if valid, return the validated (but unchanged) datetime (could skip */
   /* doing this for now, but at some point we might decide to fix up bad */
   /* string formatting as a convenience to the user, so...) */

   /* local variables */
   regex_t     myregex;
   int         myreti;
   const char *mypattern;

   mypattern =
      "^[0-9]{4}-[0-9]{2}-[0-9]{2} [0-9]{2}:[0-9]{2}:[0-9]{2}$";

   myreti = regcomp(&myregex, mypattern, REG_EXTENDED);
   if (myreti)
   {
      printf("Problem compiling myregex\n");
      exit(1);
   }
   myreti = regexec(&myregex, mydatetime, 0, NULL, 0);

   if (!myreti)
   {
      // printf("Match\n");
   }
   else if (myreti == REG_NOMATCH)
   {
      printf("bad time format -- must be \"YYYY-MM-DD HH:MM:SS\"\n");
      exit(1);
   }
}
