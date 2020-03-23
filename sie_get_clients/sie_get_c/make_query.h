#define LARGE    200

/* memory handling */
/* see https://curl.haxx.se/libcurl/c/getinmemory.html */

struct MemoryStruct
{
   char * memory;
   size_t size;
};

/* function declarations */
char *make_query(char[LARGE], char[LARGE]);

/* global variables */
extern char useproxy[];
extern char outputfilename[LARGE];
