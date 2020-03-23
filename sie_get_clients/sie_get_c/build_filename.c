#include <stdio.h>
#include <string.h>
#include "build_filename.h"

void build_filename(void)
{
   /* need these for the file name we're building */
   char string1[SMALL];
   char string2[SMALL];

   /* these are nmsgchannel format channels */
   /*@null@*/
   const char *nmsgchannels[] = { "204", "206", "207", "208",
                                  "221", NULL };

   /* NOTE: last channel in the above list MUST be NULL as shown! */

   /* replace spaces with @'s in the filenames we make */
   snprintf(string1, sizeof string1, "%s", replString(secarg, " ", "@"));
   snprintf(string2, sizeof string2, "%s", replString(thirdarg, " ", "@"));

   /* assemble the filename */
   strlcpy(outputfilename, "sie-ch", sizeof(outputfilename));
   strlcat(outputfilename, firstarg, sizeof(outputfilename));
   strlcat(outputfilename, "-{", sizeof(outputfilename));
   strlcat(outputfilename, string1, sizeof(outputfilename));
   strlcat(outputfilename, "}-{", sizeof(outputfilename));
   strlcat(outputfilename, string2, sizeof(outputfilename));
   strlcat(outputfilename, "}", sizeof(outputfilename));

   if (search_a_list(firstarg, nmsgchannels) == -1)
   {
      strlcat(outputfilename, ".jsonl", sizeof(outputfilename));
   }
   else
   {
      strlcat(outputfilename, ".nmsg", sizeof(outputfilename));
   }
}
