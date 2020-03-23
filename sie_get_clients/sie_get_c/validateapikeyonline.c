#include <string.h>
#include <stdio.h>
#include "parson.h"
#include "validateapikeyonline.h"

char *validateapikeyonline()
{
   /* check the API key for validity on the live SIE-Batch API server */

   /* local variables */
   char *      myapikeyval;
   char        params[LARGE];
   char        queryURL[LARGE];
   char *      returned_content;
   const char *status_from_json;

   JSON_Value * my_json_value;
   JSON_Object *my_json_object;

   /* get the SIE-Batch API key from local file */
   myapikeyval = getkeyfromlocalfile();

   /* assemble the parameter for checking online */
   strlcpy(params, "{\"apikey\":\"", sizeof(params));
   strlcat(params, myapikeyval, sizeof(params));
   strlcat(params, "\"}", sizeof(params));

   /* assemble the endpoint we'll visit to check */
   strlcpy(queryURL, "https://", sizeof(params));
   strlcat(queryURL, endpoint, sizeof(params));
   strlcat(queryURL, "/siebatchd/v1/validate", sizeof(params));

   /* make the query (-999 == don't write output to a file) */
   strlcpy(outputfilename, "-999", 5);
   returned_content = make_query(queryURL, params);

   /* json processing */

   /* load the returned JSON string */
   my_json_value = json_parse_string(returned_content);

   /* it initially comes in as a value, we need an object */
   my_json_object = json_object(my_json_value);

   /* extract the one value we need to confirm that the key's okay */
   status_from_json =
      json_object_get_string(my_json_object, "_status");

   return((char *)status_from_json);
} /* validateapikeyonline */
