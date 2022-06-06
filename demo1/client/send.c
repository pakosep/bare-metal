#include "ic.h"

main()
{
   ic_influx_database("exec", 8086, "ic"); /* the details of the connection to InfluxDB */
   ic_tags("host=localhost");                  /* tags to find specific data amongst the data */

   for(;;) {
       ic_measure("mystats");             /* name of a group of data */
           ic_long("busy",  42);          /* example of saving an integer */
           ic_double("pi",   3.142);      /* example of saving a floating point number */
           ic_string("state", "Running"); /* example of saving some text */
       ic_measureend();                   /* end the group */

       ic_push();      /* after many measures it is sent to InfluxDB over the network in one go*/

       sleep(60);      /* pause 60 seconds and then repeat */
}
