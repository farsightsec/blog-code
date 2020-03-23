#include <stdlib.h>
#include <stdio.h>
#include "print_usage_info.h"

void print_usage_info(void)
{
   printf("Usage:\n\n");
   printf("  sie_get_c channel \"now\" minutesBack\n");
   printf("  Example: sie_get_c 212 now 15\n\n");
   printf("OR\n\n");
   printf("  sie_get_c channel \"startdatetime\" \"enddatetime\"\n");
   printf("  Example: sie_get_c 212 \"2020-01-07 00:13:00\" \"2020-01-07 00:28:00\"\n\n");
   printf("Convenience functions:\n\n");
   printf("  Check SIE-Batch API key:                      sie_get_c checkkey\n");
   printf("  Get a listing of channels:                    sie_get_c channels\n");
   printf("  Get datetime range and volume for a channel:  sie_get_c 212\n\n");
   printf("Notes:\n");
   printf("  Datetimes are UTC and must be quoted. (Current UTC datetime: $ date -u )\n");
   printf("  Zero pad any single digit months, days, hours or minutes.\n");
   printf("  Seconds must be entered as part of the UTC datetimes (but are ignored)\n");
   printf("  Ending datetime in the future? It will be clamped to current datetime.\n\n");
   printf("  minutesBack must be >= 1 and a whole number\n");
   exit(EXIT_SUCCESS);
}
