#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <curl/curl.h>
#include "one_real_arg.h"

void one_real_arg(void)
{
   /* constants */

   const char *defined_channels[] =
   {
      "24",  "25",  "27",  "42",
      "80",  "114", "115", "204",
      "206", "207", "208", "211",
      "212", "213", "214", "221",
      NULL
   };

   /* NOTE: last entry in above list MUST be NULL as shown! */

   /* what did the user want to do? */
   if (strncmp(firstarg, "checkkey", 8) == 0)
   {
      /* check the user's key for validity and exit */
      printf("API key status is %s\n", validateapikeyonline());
      exit(EXIT_SUCCESS);
   }
   else if (strncmp(firstarg, "channels", 8) == 0)
   {
      /* list channels for the user and exit */
      (void)list_channels();
      exit(EXIT_SUCCESS);
   }
   else
   {
      /* The only other options should be a specific numeric channel */

      /* non-numeric single argument (unknown command) */
      if (!(isNumeric(firstarg)))
      {
         printf("not a channel number nor a known command\n");
         exit(EXIT_FAILURE);
      }

      /* channel number's too big or too small */
      if ((atoi(firstarg) < 10) || (atoi(firstarg) > 255))
      {
         printf("out of range SIE-Batch API channel\n");
         exit(EXIT_FAILURE);
      }

      /* did the user ask for a channel this script doesn't know about? */
      if (search_a_list(firstarg, defined_channels) == -1)
      {
         printf("Channel not currently available from this script\n");
         exit(EXIT_FAILURE);
      }

      /* get earliest available datetime, latest available datetime */
      /* and the volume that range represents for the specified channel */

      /* retrieve details about a specific channel the user asked about */
      (void)printout_intervals();
      exit(EXIT_SUCCESS);
   }
} /* one_real_arg */
