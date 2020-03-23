#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "check_channel.h"

void check_channel(void)
{
   long requested_start_seconds;
   long requested_stop_seconds;

   long earliest_date_seconds;
   long latest_date_seconds;

   /* make sure that the channel is available and the dates are in-range */

   /* check the available datetime range for this channel */
   (void)check_intervals();

   /* convert the requested and available datetimes into Unix seconds */
   /* (easier to compare dates as Unix epoch seconds */
   requested_start_seconds = string_fmt_time_to_seconds(secarg);
   earliest_date_seconds   = string_fmt_time_to_seconds(
      earliestdate_avail);

   requested_stop_seconds = string_fmt_time_to_seconds(thirdarg);

   latest_date_seconds = string_fmt_time_to_seconds(
      latestdate_avail);

   /* REQUIRD: requested ending time can't be in the future */
   if (requested_stop_seconds > latest_date_seconds)
   {
      printf("Stop time can't be in the future!\n");
      exit(EXIT_FAILURE);
   }

   /* REQUIRED: start datetime must be earlier than stop datetime */
   if ((requested_stop_seconds - requested_start_seconds) < 0)
   {
      printf("Start datetime must be earlier than stop datetime\n");
      exit(EXIT_FAILURE);
   }

   /* REQUIRED: start datetime must be after the earliest data available */
   if ((requested_start_seconds - earliest_date_seconds) < 0)
   {
      printf("Start datetime out of range. Must be no earlier than\n");
      printf("%s\n", earliestdate_avail);
      exit(EXIT_FAILURE);
   }
} /* check_channel */
