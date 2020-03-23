/* sie_get.c */

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <curl/curl.h>

/* write_data function from https://curl.haxx.se/libcurl/c/url2file.html */
/* static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{ size_t  written = fwrite(ptr, size, nmemb, (FILE *) stream);
  return written; }
*/

/* function declarations */
char * getAPIkey(void);

/* main **************************************************************** */

#define BIG 200
#define EVENBIGGER 500

int main(int argc, char **argv)
{ 

  /* declarations (alphabetically ordered by variable name) */

  char arguments[EVENBIGGER];
  int chanflag = 0;
  CURL *curl_handle;
  char currentGMTtimestring[BIG];
  char enddatetime[BIG];
  time_t epoch;
  char *filetype = NULL;
  const char *fmt = "%Y-%m-%d %H:%M:%S";
  char *fullcommand;
  struct tm *gmt;
  unsigned long i;
  size_t lengthofstring1;
  size_t lengthofstring2;
  char *myapikey = NULL;
  int mysecondsback = 0;
  static char *nowliteral = "now";
  FILE *outputfile;
  char outputfilename[BIG];
  CURLcode res;
  char startdatetime[BIG];
  struct tm *start_from_offset_val;
  char string1[BIG];
  char string2[BIG];
  time_t t;
  long http_code = 0;

  /* if we don't have the number of arguments we expect, provide precis */
  if (argc != 4) {
     printf ("\nsie_get\n\n");
     printf ("ERROR: Wrong number of command line arguments supplied.\n\n");
     printf ("  Usage:   sie_get channel \"startdatetime\" \"enddatetime\"\n");
     printf ("  Example: sie_get 212 \"2020-01-07 00:13:00\" \"2020-01-07 00:28:00\"\n");
     printf ("  Notes:   UTC datetimes must within a day or so and must be double quoted.\n");
     printf ("           Zero pad any single digit months, days, hours or minutes.\n");
     printf ("           Seconds must be part of the UTC datetimes (but are ignored.)\n");
     printf ("           Need the current UTC time? Try $ date -u\n");
     printf ("or\n\n");
     printf ("  Usage:   sie_get channel \"now\" minutesBack\n");
     printf ("  Example: sie_get 212 now 20\n"); 
     printf ("  Note:    Generally: 0 < minutesBack <= 1440 (and integer valued)\n\n");
     printf ("TERMINATED\n\n");
     exit(EXIT_FAILURE);
   }

  /* get the command line arguments */
  chanflag = atoi(argv[1]);
  char *chanflagstring = strdup(argv[1]);
  strcpy(startdatetime, argv[2]);
  strcpy(enddatetime, argv[3]);

  /* save the current UTC time */
  t = time(NULL);
  gmt = gmtime(&t);
  strftime(currentGMTtimestring, sizeof(currentGMTtimestring), fmt, gmt);

  /* get and validate the user's SIE-Batch API key */
  myapikey = getAPIkey();

  /* allow either specify absolute datetime strings, or "now" and */
  /* integer minutesBack. this block handles the 2nd case */

  if (strcmp(startdatetime,nowliteral) == 0) {
     /* enddatetime is actually minutesBack field saying how far back
        we want to go from "now", so we save it as such in a new int var */
     mysecondsback = atoi(enddatetime)*60;

     /* set the endtime to the current GMT time (as a human datetime) */
     /* (replacing the number of minutes specified by the user) */
     strcpy(enddatetime, currentGMTtimestring);
     strcpy(string2, currentGMTtimestring);

     /* set the start time to the calculated earlier human datetime, */
     /* replacing "now" */
     epoch = t - mysecondsback;
     start_from_offset_val = gmtime(&epoch);
     strftime(startdatetime, sizeof(startdatetime), fmt, start_from_offset_val);
     strcpy(string1, startdatetime);

  } else {

     /* in the explicit form, we've already startdatetime and enddatetime */
     /* we just need a copy we can munge to form the filename */
     strcpy(string1, startdatetime);
     strcpy(string2, enddatetime);

  }

  /* don't like spaces in filenames. (this is a framework for fixing other */
  /* unwanted chars, should we need to do so, too). we'll fix start and end */

  strcpy(string1, startdatetime);
  lengthofstring1=strlen(string1);
  for (i = 0; i <= lengthofstring1; i++) {
     if (string1[i] == ' ') string1[i] = '@';
  }

  strcpy(string2, enddatetime);
  lengthofstring2=strlen(string2);
  for (i = 0; i <= lengthofstring2; i++) {
       if (string2[i] == ' ') string2[i] = '@';
  }

  /* we want a file extension reflective of the file's contents. A number */
  /* of channels use native NMSG format, the rest are JSON Lines */

  if ((chanflag == 204) ||
      (chanflag == 206) ||
      (chanflag == 207) ||
      (chanflag == 208) ||
      (chanflag == 221))
    filetype = ".nmsg";
  else filetype = ".jsonl";

  /* now build the output filename by concatenating elements into a name */
  strcpy(outputfilename, "sie-ch");
  strcat(outputfilename, chanflagstring);
  strcat(outputfilename, "-{");
  strcat(outputfilename, string1);
  strcat(outputfilename, "}-{");
  strcat(outputfilename, string2);
  strcat(outputfilename, "}");
  strcat(outputfilename, filetype);

  /* initialize curl. this must be called once and only once. */
  if (curl_global_init(CURL_GLOBAL_ALL) != 0) {
    printf("curl_global_init() failed\n");
    exit(EXIT_FAILURE);
  }

  /* the POST we're going to be doing needs some arguments to work */
  /* we'll assemble those arguments now */
  strcpy(arguments, "{\"apikey\":\"");
  strcat(arguments, myapikey);
  strcat(arguments, "\",\"channel\":");
  strcat(arguments, chanflagstring);
  strcat(arguments, ",\"start_time\":\"");
  strcat(arguments, startdatetime);
  strcat(arguments, "\",\"end_time\":\"");
  strcat(arguments, enddatetime);
  strcat(arguments, "\"}"); 

  printf("about to do the curl_easy_init\n");
  curl_handle = curl_easy_init();
  printf("just did the the curl_easy_init\n");

  fullcommand="https://stage-batch.dev.fsi.io/siebatchd/v1/siebatch/chfetch";

  printf("about to do the curl_easy_setopt curlopt_url\n");
  curl_easy_setopt(curl_handle, CURLOPT_URL, fullcommand);
  printf("just did the curl_easy_setopt curlopt_url\n");

  printf("about to do the curlopt_postfields\n");
  curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, arguments);
  printf("just did the curlopt_postfields\n");

  printf("about to do the curlopt_followlocation\n");
  curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, true);
  printf("just did the curlopt_followlocation\n");

  printf("about to do the curlopt_writefunction\n");
  /* curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_data); */
  printf("just did the curlopt_writefunction\n");

  outputfile = fopen(outputfilename, "wb");
  if (outputfile) {
    /* write the page body to this file handle */
    printf("about to do the curlopt_writedata\n");
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, outputfile);
    printf("just did the curlopt_writedata\n");

    double tries = 0;
    double elapsed = 0;
    bool done = false;

    while (tries != 3 && !done) {
      /* do the actual curl command */

      printf("about to do the curl_easy_perform\n");
      res = curl_easy_perform(curl_handle);
      printf("just did the curl_easy_perform\n");

      printf("about to do the CURLINFO_EFFECTIVE_URL\n");
      curl_easy_getinfo(curl_handle, CURLINFO_EFFECTIVE_URL, &fullcommand);
      printf("just did the CURLINFO_EFFECTIVE_URL\n");

      printf("about to do the CURLINFO_RESPONSE_CODE\n");
      curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &http_code);
      printf("just did the CURLINFO_RESPONSE_CODE\n");

      printf("about to do the CURLINFO_TOTAL_TIME\n");
      curl_easy_getinfo(curl_handle, CURLINFO_TOTAL_TIME, &elapsed);
      printf("just did the CURLINFO_TOTAL_TIME, elapsed=%f\n", elapsed);

      curl_off_t cl;
      res = curl_easy_getinfo(curl_handle, 
          CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &cl);
      if(!res) {
        printf("Download size: %" CURL_FORMAT_CURL_OFF_T "\n", cl);
      }

      if (res != CURLE_OK || http_code != 200) {
         tries++;
         printf("tries = %f\n",tries);
         }
      else
         {
         printf("completed successfully\n");
         done = true;
         }

      if (res != CURLE_OK || http_code != 200) {
         if (tries == 3) {
            /* too many tries */
            return(EXIT_FAILURE);
         } else {
           /* curl failed, retry */
           printf ("retrying... %f\n", tries);
         }
       }
    }

   fclose(outputfile);
  }
  
  curl_easy_cleanup(curl_handle);
  curl_global_cleanup();
  free(myapikey);
  return (EXIT_SUCCESS);
}
