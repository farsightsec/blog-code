#include <string.h>
#include <stdio.h>
#include <locale.h>
#include <stdlib.h>
#include "parson.h"   /* https://github.com/kgabis/parson/ */
#include "check_intervals.h"

void check_intervals()
{
   char  chan_to_check[8];
   char  earliest[SMALL];
   char  latest[SMALL];
   char *myapikeyval;
   char  other_format[8];
   char  params[LARGE];
   char *response;
   char  url[LARGE];

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

   /* extract the two values we actually care about */
   /* NOT printing anything here , just load it into the struct */
   strlcpy(earliestdate_avail,
           json_object_get_string(my_obj_2, "earliest"),
           sizeof(earliest));
   strlcpy(latestdate_avail,
           json_object_get_string(my_obj_2, "latest"),
           sizeof(latest));
} /* check_intervals */
