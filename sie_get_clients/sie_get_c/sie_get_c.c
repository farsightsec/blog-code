#include <stdlib.h>
#include <string.h>
#include "sie_get_c.h"

int main(int argc, char *argv[])
{
   /* prototypes */
   void three_real_args(void);
   void one_real_arg(void);
   void print_usage_info(void);

   /* argc=4 --> three arguments */
   /* argc=2 --> single argument */
   /* argc=1 --> no arguments */

   if (argc == 4)
   {
      /* we'll pulling actual data */

      /* load the globals */
      strlcpy(firstarg, argv[1], sizeof(firstarg));
      strlcpy(secarg, argv[2], sizeof(firstarg));
      strlcpy(thirdarg, argv[3], sizeof(firstarg));

      (void)three_real_args();
   }
   else if (argc == 2)
   {
      /* checkkey, channels or an actual channel number provided */

      /* save that info as a global */
      strlcpy(firstarg, argv[1], sizeof(firstarg));

      (void)one_real_arg();
   }
   else
   {
      /* wrong number of arguments, provide precis and bail */

      (void)print_usage_info();
      exit(EXIT_FAILURE);
   }
} /* main */
