/* https://rosettacode.org/wiki/Search_a_list#C */

#include <stdio.h>
#include <string.h>

int search_a_list(const char *needle, const char **hs)
{
   int i = 0;

   while (hs[i] != NULL)
   {
      if (strcmp(hs[i], needle) == 0)
      {
         return(i);
      }

      i++;
   }
   return(-1);
}
