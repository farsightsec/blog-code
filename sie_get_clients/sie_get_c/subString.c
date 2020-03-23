#include <string.h>
#include <stdlib.h>

/* get substring */
/* stackoverflow.com/questions/2114377/strings-in-c-how-to-get-substring */

/* prototype */
char *subString(char *, int, int, char *);

char *subString(char *input, int offset, int len, char *dest)
{
   int input_len = strlen(input);

   if (offset + len > input_len)
   {
      return(NULL);
   }

   strncpy(dest, input + offset, len);
   return(dest);
}
