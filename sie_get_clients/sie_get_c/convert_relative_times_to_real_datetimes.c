#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "convert_relative_times_to_real_datetimes.h"

void convert_relative_times_to_real_datetimes()
{
   /* local variables */

   long      startseconds;
   long      endingtime_seconds;
   long      extraseconds;
   struct tm gmendingtime;
   long      mysecondsback;

   char *myformat;
   char  lasttwo[3];

   myformat = "%Y-%m-%d %H:%M:%S";

   /* time_t is raw epoch seconds */
   time_t epochseconds;

   /* save a copy of the number of minutes worth of data we want to go */
   /* back in relative time */
   strlcpy(saved_minutes_back, thirdarg, sizeof(saved_minutes_back));

   /* when retrieving data, there are two options: */
   /* */
   /* one option is relative times; if we get one, make it a real time */
   /* */
   /* in relative format, the initial "ending time" is actually the minutes */
   /* worth of data we want to retrieve */
   /* */
   /* the "real" ending datetime will be created from the current GMT time */
   /* we will be doing math on the epoch seconds */

   epochseconds = time(NULL);
   gmendingtime = *gmtime(&epochseconds);

   /* now compute the formatted ending date time in standard  */
   /* YYYY-MM-DD HH:MM:SS */
   strftime(thirdarg, SMALL, myformat, &gmendingtime);

   /* find just the seconds from that string (we need the starting point) */
   lasttwo[0] = 0;  /* make sure the string is NULL terminated */
   subString(thirdarg, (strlen(thirdarg) - 2), 2, lasttwo);

   extraseconds = atol(lasttwo);
   /* subtract the seconds from the full datetime to end up with 00 seconds */
   endingtime_seconds = timegm(&gmendingtime) - extraseconds;

   /* let's now work on the starting time */
   /* we compute the "real" starting datetime by offsetting backwards */
   /* our to-be-modified datetime is in epoch seconds, so convert min */
   /* to seconds */

   mysecondsback = atol(saved_minutes_back) * 60;
   startseconds  = endingtime_seconds - mysecondsback;
   struct tm ts = *gmtime(&startseconds);
   strftime(secarg, SMALL, myformat, &ts);
   strftime(thirdarg, SMALL, myformat, gmtime(&endingtime_seconds));
} /* convert_relative_times_to_real_datetimes */
