#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <curl/curl.h>
#include <fcntl.h>
#include <unistd.h>
#include "make_query.h"

/* memory handling */
/* see https://curl.haxx.se/libcurl/c/getinmemory.html */

static size_t WriteMemoryCallback(void *contents, size_t size,
                                  size_t nmemb, void *userp)
{
   size_t realsize          = size * nmemb;
   struct MemoryStruct *mem = (struct MemoryStruct *)userp;

   char *ptr = realloc(mem->memory, mem->size + realsize + 1);

   if (ptr == NULL)
   {
      /* out of memory! */
      printf("not enough memory (realloc returned NULL)\n");
      return(0);
   }

   mem->memory = ptr;
   memcpy(&(mem->memory[mem->size]), contents, realsize);
   mem->size += realsize;
   mem->memory[mem->size] = 0;

   return(realsize);
}

char *make_query(char url[LARGE], char params[LARGE])
{
   /* variables */

   CURL *   mycurl;
   CURLcode res;
   FILE *   f = NULL;

   long http_code = 0;

   char errbuf[CURL_ERROR_SIZE];
   struct curl_slist * list = NULL;
   struct MemoryStruct chunk;

   /* the libcurl example code requires use of this malloc/realloc */
   chunk.memory = malloc(1);  /* grown as needed by the realloc above */
   chunk.size   = 0;          /* no data at this point */

   double tries   = 0;
   double elapsed = 0;
   bool   done    = false;

   if (strncmp(outputfilename, "-999", 5) != 0)
   {
      f = fopen(outputfilename, "wb");
   }

   mycurl = curl_easy_init();

   /* if needed for debugging */
   /*  curl_easy_setopt(mycurl, CURLOPT_VERBOSE, 1L); */

   /* the end point we're gong to visit */
   curl_easy_setopt(mycurl, CURLOPT_URL, url);

   /* force IPv4 only for now */
   curl_easy_setopt(mycurl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);

   /* we're doing a POST */
   curl_easy_setopt(mycurl, CURLOPT_POST, true);

   /* pass our arguments to the query */
   curl_easy_setopt(mycurl, CURLOPT_POSTFIELDS, params);

   /* redirect if need be */
   curl_easy_setopt(mycurl, CURLOPT_FOLLOWLOCATION, 1L);

   /* we pass an HTTP header to ensure it's a JSON world */
   list = curl_slist_append(list, "Content-Type: application/json");
   curl_easy_setopt(mycurl, CURLOPT_HTTPHEADER, list);

   /* handle timeouts */
   curl_easy_setopt(mycurl, CURLOPT_CONNECTTIMEOUT, 30L);
   curl_easy_setopt(mycurl, CURLOPT_TIMEOUT, 3000L);
   curl_easy_setopt(mycurl, CURLOPT_USERAGENT, "sie_get_c/1.0");

   /* connect via a proxy or connect directly with SSL/TLS? */
   if (strcmp(useproxy, "yes") == 0)
   {
      /* printf("trying to use the proxy...\n"); */
      curl_easy_setopt(mycurl, CURLOPT_PROXY, "127.0.0.1");
      curl_easy_setopt(mycurl, CURLOPT_PROXYPORT, 1080);
      curl_easy_setopt(mycurl, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5);
   }
   else
   {
      /* printf("trying to connect directly...\n"); */
      curl_easy_setopt(mycurl, CURLOPT_SSL_VERIFYPEER, 1);
      curl_easy_setopt(mycurl, CURLOPT_SSL_VERIFYHOST, 1);
   }

   /* write the actual data files directly to the outputfile */
   /* or for (apikey check, channel listing, etc.) we're just */
   /* going to write to a buffer (and then return that)) */
   if (strncmp(outputfilename, "-999", 5) == 0)
   {
      /* printf("writing to the function, not to a file\n"); */
      /* send all data to this function  */
      curl_easy_setopt(mycurl, CURLOPT_WRITEFUNCTION,
                       WriteMemoryCallback);
      /* we pass our 'chunk' struct to the callback function */
      curl_easy_setopt(mycurl, CURLOPT_WRITEDATA,
                       (void *)&chunk);
   }
   else
   {
      /* printf("writing directly to the file\n"); */
      curl_easy_setopt(mycurl, CURLOPT_WRITEDATA, f);
   }

   /* try the query three times, just in case there's an */
   /* intermittent issue */
   while (tries != 3 && !done)
   {
      /* do the actual curl command */
      res = curl_easy_perform(mycurl);
      /* printf("after curl_easy_perform, res=%d\n",res); */

      /* was the request successful? */
      curl_easy_getinfo(mycurl, CURLINFO_RESPONSE_CODE, &http_code);
      /* printf("Response code=%ld\n", http_code); */

      if (res != CURLE_OK)
      {
         size_t len = strlen(errbuf);
         fprintf(stderr, "\nlibcurl: (%d) ", res);
         if (len)
         {
            fprintf(stderr, "%s%s", errbuf,
                    ((errbuf[len - 1] != '\n') ? "\n" : ""));
         }
         else
         {
            fprintf(stderr, "%s\n", curl_easy_strerror(res));
         }
      }

      /* how long did it take? */
      curl_easy_getinfo(mycurl, CURLINFO_TOTAL_TIME, &elapsed);
      /* printf("Elapsed time=%f\n", elapsed); */

      /* how big's the download? we primarily care about short downloads */
      curl_off_t cl;
      res = curl_easy_getinfo(mycurl,
                              CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &cl);
      if (!res)
      {
         /* printf("Download size: %" CURL_FORMAT_CURL_OFF_T "\n", cl); */
      }

      if (res != CURLE_OK || http_code != 200)
      {
         printf("tries = %f\n", tries);
      }
      else
      {
         done = true;
      }
   }

   curl_easy_cleanup(mycurl);
   curl_global_cleanup();

   if (strncmp(outputfilename, "-999", 5) == 0)
   {
      /* return results from the query */
      return(chunk.memory);
   }
   else
   {
      /* this means that we've written a file with our output */
      exit(0);
   }
} /* make_query */
