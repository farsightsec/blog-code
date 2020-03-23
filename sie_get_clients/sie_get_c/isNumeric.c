#include <ctype.h>
#include <stdlib.h>

/* see https://rosettacode.org/wiki/Determine_if_a_string_is_numeric#C */

int isNumeric(const char *s)
{
   if (s == NULL || *s == '\0' || isspace(*s))
   {
      return(0);
   }

   char *p;
   strtod(s, &p);
   return(*p == '\0');
}
