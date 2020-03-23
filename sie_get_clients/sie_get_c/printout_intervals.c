#include <string.h>
#include <stdio.h>
#include <locale.h>
#include <stdlib.h>
#include "parson.h"      /* https://github.com/kgabis/parson/ */
#include "printout_intervals.h"

void printout_intervals()
{
   char  chan_to_check[8];
   char  earliest[SMALL];
   char  fmtfirstarg[8];
   char  latest[SMALL];
   char *myapikeyval;
   char  other_format[8];
   char  params[LARGE];
   char *response;
   char  url[LARGE];
   char  volume_string[SMALL];

   double bigness;

   JSON_Object *my_obj_1;
   JSON_Object *my_obj_2;

   JSON_Value *my_jv;

   myapikeyval = getkeyfromlocalfile();

   /* build our parameters for the query */
   /* {"channels":[212],"apikey":"blah"}  (note: no quotes around [chan]) */
   strlcpy(chan_to_check, "[", sizeof(chan_to_check));
   strlcat(chan_to_check, firstarg, sizeof(chan_to_check));
   strlcat(chan_to_check, "]", sizeof(chan_to_check));

   strlcpy(params, "{\"apikey\":\"", sizeof(params));
   strlcat(params, myapikeyval, sizeof(params));
   strlcat(params, "\",\"channels\":", sizeof(params));
   strlcat(params, chan_to_check, sizeof(params));
   strlcat(params, "}", sizeof(params));

   /* build the endpoint we're going to visit */
   strlcpy(url, "https://", sizeof(params));
   strlcat(url, endpoint, sizeof(params));
   strlcat(url, "/siebatchd/v1/siebatch/chdetails", sizeof(params));

   /* actually do the query */
   /* -999 is a magic value meaning no file output */
   strlcpy(outputfilename, "-999", 5);
   response = make_query(url, params);

   /* Now process the JSON results */

   /* get the json string, parse it, and extract the channels and descrp */
   my_jv = json_parse_string(response);

   /* it's all under the channels leg of the JSON tree */
   my_obj_1 = json_object_get_object(json_object(my_jv), "channels");

   /* to pick a chan,  we need the number w/o brackets but with ch */
   strlcpy(other_format, "ch", sizeof(other_format));
   strlcat(other_format, firstarg, sizeof(other_format));

   /* we're now down a specific channel number in the JSON tree */
   my_obj_2 = json_object_get_object(my_obj_1, other_format);

   /* extract the three values we actually care about */
   strlcpy(earliest, json_object_get_string(my_obj_2, "earliest"),
           sizeof(earliest));
   strlcpy(latest, json_object_get_string(my_obj_2, "latest"),
           sizeof(latest));
   /* this one's numeric, not a string */
   bigness = json_object_get_number(my_obj_2, "size");

   /* format the volume with thousands commas */
   setlocale(LC_NUMERIC, "");
   if (bigness >= 999.0)
   {
      snprintf(volume_string, SMALL, "%'d", (int)bigness);
   }

   /* could add a header, but it's pretty self-obvious, right? */
   /* printf('chan  earliest datetime    latest datetime       octets\n') */

   /* strip the square brackets from the channel number for output*/
   strlcpy(fmtfirstarg, replString(firstarg, "[", ""),
           sizeof(fmtfirstarg));
   strlcpy(fmtfirstarg, replString(fmtfirstarg, "]", ""),
           sizeof(fmtfirstarg));

   printf("%s  \"%s\"  \"%s\"  %s\n",
          fmtfirstarg, earliest, latest, volume_string);
} /* printout_intervals */
