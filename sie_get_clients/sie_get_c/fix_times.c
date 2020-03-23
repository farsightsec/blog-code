#include <stdio.h>
#include <strings.h>
#include "fix_times.h"

void fix_times(void)
{
   /* handles calling the rest of the routines to fix up times */
   /* arguments come in from the command line as global variables */
   /* so we don't pass them in */

   /* if relative times, replace the ending time with the current GMT time */
   /* set the starting time back by the specified number of minutes */

   if (strncmp(secarg, "now", 4) == 0)
   {
      (void)convert_relative_times_to_real_datetimes();
   }
   else
   {
      /* we have real timedate stamps for starting and ending datetimes */
      /* process the starting datetime value... */
      /* correctly written datetime value? */

      (void)validate_input_time_date_format(secarg);

      /* repeat for the ending datetime value... */
      (void)validate_input_time_date_format(thirdarg);
   }
}
