#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <curl/curl.h>
#include "parson.h"    /* https://github.com/kgabis/parson/ */
#include "three_real_args.h"

void three_real_args(void)
{
   char  params[LARGE];
   char  queryURL[LARGE];
   char *myapikey;

   /* make sure the times are sane (right format and not in the future) */
   (void)fix_times();

   /* make sure the times are in-range for this particular channel */
   (void)check_channel();

   /* construct the output filename from the channel and dates */
   (void)build_filename();

   /* retrieve the API key we need */
   myapikey = getkeyfromlocalfile();

   /* construct the parameters we're going to pass-in */
   strlcpy(params, "{\"apikey\":\"", sizeof(params));
   strlcat(params, myapikey, sizeof(params));
   strlcat(params, "\",\"channel\":", sizeof(params));
   strlcat(params, firstarg, sizeof(params));
   strlcat(params, ",\"start_time\":\"", sizeof(params));
   strlcat(params, secarg, sizeof(params));
   strlcat(params, "\", \"end_time\":\"", sizeof(params));
   strlcat(params, thirdarg, sizeof(params));
   strlcat(params, "\"}", sizeof(params));

   strlcpy(queryURL, "https://", sizeof(queryURL));
   strlcat(queryURL, endpoint, sizeof(queryURL));
   strlcat(queryURL, "/siebatchd/v1/siebatch/chfetch", sizeof(queryURL));

   (void)make_query(queryURL, params);

   exit(EXIT_SUCCESS);
} /* three_real_args */
