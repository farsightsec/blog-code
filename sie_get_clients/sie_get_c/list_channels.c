#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "parson.h"
#include "list_channels.h"

void list_channels()
{
   /* no real args come in because we always just want to list all channels */

   char queryURL[LARGE];
   char params[LARGE];

   char *returned_content;
   char *myapikeyval;

   myapikeyval = getkeyfromlocalfile();

   strlcpy(params, "{\"apikey\":\"", sizeof(params));
   strlcat(params, myapikeyval, sizeof(params));
   strlcat(params, "\"}", sizeof(params));

   strlcpy(queryURL, "https://", sizeof(queryURL));
   strlcat(queryURL, endpoint, sizeof(queryURL));
   strlcat(queryURL, "/siebatchd/v1/validate", sizeof(queryURL));

   strlcpy(outputfilename, "-999", 5);
   returned_content = make_query(queryURL, params);

   /* json processing */

   struct keys { char key[7]; };
   struct vals { char val[70]; };

   struct keys mykeys[24];
   struct vals myjvals[24];

   JSON_Value *my_jv;

   JSON_Object *my_obj_1;
   JSON_Object *my_obj_2;

   int j_i;
   int j_j;

   char tempkey[7];
   char tempval[70];

   /* parse the json string, extract and print the channels and descrips */
   my_jv = json_parse_string(returned_content);

   /* the data we need is under profile.siebatch */
   /* NOTE: do NOT get distracted by the profile.channels object! */
   my_obj_1 = json_object_dotget_object(json_object(my_jv),
                                        "profile.siebatch");

   /* pull a list of channel numbers */
   for (j_i = 0; j_i < json_object_get_count(my_obj_1); j_i++)
   {
      /* we know how many objects now, but what are the object names? */
      sprintf(mykeys[j_i].key, "%s", json_object_get_name(my_obj_1, j_i));

      /* now that we've got the names, step through those objects */
      my_obj_2 = json_object_get_object(my_obj_1, mykeys[j_i].key);

      /* the channel description's the last thing we need */
      sprintf(myjvals[j_i].val, "%s",
              json_object_get_string(my_obj_2, "description"));
   }

   /* the channel names are prefixed with ch, remove that for sorting */
   for (j_i = 0; (j_i < json_object_get_count(my_obj_1)); j_i++)
   {
      strlcpy(tempkey, replString(mykeys[j_i].key, "ch", ""),
              sizeof(tempkey));
      /* now restore the ch-less keys to their original location */
      strlcpy(mykeys[j_i].key, tempkey, sizeof(mykeys[j_i]));
   }

   /* crumby little bubblesort (works fine for a tiny hash like this one) */
   for (j_j = 0; (j_j < json_object_get_count(my_obj_1) - 1); j_j++)
   {
      for (j_i = 0; (j_i < (json_object_get_count(my_obj_1) - j_j - 1)); j_i++)
      {
         if (atoi(mykeys[j_i].key) > atoi(mykeys[j_i + 1].key))
         {
            strlcpy(tempkey, mykeys[j_i].key, sizeof(tempkey));
            strlcpy(tempval, myjvals[j_i].val, sizeof(tempval));

            strlcpy(mykeys[j_i].key, mykeys[j_i + 1].key, sizeof(mykeys[j_i].key));
            strlcpy(myjvals[j_i].val, myjvals[j_i + 1].val, sizeof(myjvals[j_i].val));

            strlcpy(mykeys[j_i + 1].key, tempkey, sizeof(mykeys[j_i].key));
            strlcpy(myjvals[j_i + 1].val, tempval, sizeof(myjvals[j_i].val));
         }
      }
   }

   /* print the actual channel listing */
   for (j_i = 0; (j_i < json_object_get_count(my_obj_1)); j_i++)
   {
      printf("ch%s  %s\n", mykeys[j_i].key, myjvals[j_i].val);
   }
   exit(EXIT_SUCCESS);
} /* list_channels */
