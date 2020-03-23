/* getAPIkey */

#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <time.h>
#include <json.h>

size_t len = 0;
int apikeylen;
FILE *stream;
char *myapikey;
CURL *curl_handle;
CURLcode res;
char arguments[500];
char *arguments1 = "{\"apikey\":\"";
char *arguments2 = NULL;
char *arguments3 = "\"}";

/* download to memory: see  https://curl.haxx.se/libcurl/c/getinmemory.html */

struct MemoryStruct {
  char *memory;
  size_t size;
};

static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if(ptr == NULL) {
    /* out of memory! */ 
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
 
  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}

/* prototype */
char * getAPIkey (void);

char * getAPIkey (void)
{

  /* declarations (in alphabetical order) */
  static char *apikeybasefilename = ".sie-get-key.txt";
  static char *arguments1="{\"apikey\":\"";
  struct MemoryStruct chunk;
  char *filepath = NULL;
  static char *fullcommand="https://siebatch.it.fsi.io/siebatchd/v1/validate";
  char *home_dir = NULL;
  struct json_object *new_obj;
  struct json_object *new_obj_extract;
  size_t size;
  static char *slash = "/";
  int status_code=0;

  home_dir = getenv("HOME");

  size = strlen(home_dir) + strlen(slash) +
         strlen(apikeybasefilename) + 1;
  filepath = malloc(size);
  snprintf(filepath, size, "%s%s%s", home_dir, slash, apikeybasefilename);

  chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */ 
  chunk.size = 0;    /* no data at this point */ 

  stream = fopen(filepath, "r");
  if (stream == NULL) {
    printf("ERROR: no API key file at %s\n", filepath);
    exit(EXIT_FAILURE);
  }
  getline(&myapikey, &len, stream);
  fclose(stream);

  /* clean up the sie-batch apikey */
  /* remove newline if present */
  apikeylen = strlen(myapikey);
  if (myapikey[apikeylen - 1] == '\n')
    myapikey[apikeylen - 1] = 0;

  /* validate sie-batch apikey */
  curl_handle = curl_easy_init();
  strcpy(arguments, arguments1);
  strcat(arguments, myapikey);
  strcat(arguments, arguments3);

  curl_easy_setopt(curl_handle, CURLOPT_URL, fullcommand);

  /* send all data to this function  */ 
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
 
  /* we pass our 'chunk' struct to the callback function */ 
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

  curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, arguments);
  res = curl_easy_perform(curl_handle);

  if (res != CURLE_OK) {
     printf("curl_easy_perform() failed: %s\n",
            curl_easy_strerror(res));
  } else {
     new_obj = json_tokener_parse(chunk.memory);

     new_obj_extract = json_object_object_get(new_obj, "status");
     status_code=json_object_get_int(new_obj_extract);
     if (status_code != 0) {
        printf("%d ", status_code);
        new_obj_extract = json_object_object_get(new_obj, "_message");
        printf("%s\n", json_object_to_json_string(new_obj_extract));
     }
  }
  curl_easy_cleanup(curl_handle);
  free(chunk.memory);
  curl_global_cleanup();
  free(filepath);
  if (status_code != 0) exit(EXIT_FAILURE);
  return myapikey;
}
