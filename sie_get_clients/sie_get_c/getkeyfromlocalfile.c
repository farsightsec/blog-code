#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "getkeyfromlocalfile.h"

char *getkeyfromlocalfile()
{
   /* Retrieves the SIE-Batch API key */

   if ((homedir = getenv("HOME")) == NULL)
   {
      homedir = getpwuid(getuid())->pw_dir;
   }

   strlcpy(filepath, homedir, sizeof(filepath));
   strlcat(filepath, "/.sie-get-key.txt", sizeof(filepath));

   if (access(filepath, F_OK) == -1)
   {
      printf("\nERROR:\n\n  No SIE-Batch API keyfile at ~/.sie-get-key.txt\n");
      exit(1);
   }

   f          = fopen(filepath, "r");
   linelength = getline(&myapikey, &len, f);

   if (myapikey[linelength - 1] == '\n')
   {
      myapikey[linelength - 1] = 0;
   }

   fclose(f);

   return(myapikey);
}
