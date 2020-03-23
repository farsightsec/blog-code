#include <string.h>
#include <stdio.h>
#include <time.h>
#include "string_fmt_time_to_seconds.h"

long string_fmt_time_to_seconds(char *string_format_time)
{
   /* utility function to convert a string format time to epoch seconds */

   strptime(string_format_time, "%Y-%m-%d %H:%M:%S", &dt);
   epoch_seconds = mktime(&dt);
   return(epoch_seconds);
}
