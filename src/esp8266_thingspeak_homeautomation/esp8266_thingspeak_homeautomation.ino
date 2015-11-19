#include <ESP8266WiFi.h>

#define COMP          1
#define HOME          0
#define DEBUG_SERIAL  1

#if COMP
  #define WIFI_SSID   "MISFIT SW"
  #define WIFI_PASS   "Wearables1!"
#elif HOME
  #define WIFI_SSID   "Hong_quan"
  #define WIFI_PASS   "0908108060a"
#endif

#define THINGSPEAK_HOST                     "api.thingspeak.com"
  
#define THINGSPEAK_LIGHT_NO1_CHANNEL        "66225"
#define THINGSPEAK_LIGHT_NO1_READ_KEY       "X1ZDQFFMYXEVQB7G"

#define THINGSPEAK_LIGHT_NO1_LOG_CHANNEL    "66636"
#define THINGSPEAK_LIGHT_NO1_LOG_WRITE_KEY  "V65MD3S4STAVNEUB"

/* Http request example
 * GET /update?key=F9I3HN9MA22DOE1L&field1=324 HTTP/1.1
 * Host: api.thingspeak.com
 */
#define THINGSPEAK_FORM_HTTP_WRITE_1_FIELD_REQUEST(key, str_value)  \
        ("GET /update?key=" + key + "&field1=" + str_value + " HTTP/1.1\nHost: " + THINGSPEAK_HOST + "\r\n\r\n")
        
/* Http request example
 * GET /channels/66225/feed.json?key=CHO2NZG9CGGVVWPA HTTP/1.1
 * Host: api.thingspeak.com
 */
#define THINGSPEAK_FORM_HTTP_READ_ALL_FEEDS_REQUEST(channel, key)   \
        ("GET /channels" + channel + "/feed.json?key=" + key + " HTTP/1.1\nHost: " + THINGSPEAK_HOST + "\r\n\r\n")

/* Http request example
 * DELETE /channels/66225/feeds/?api_key=BVZUYDAEPYX7OUMX HTTP/1.1
 * Host: api.thingspeak.com
 */
#define THINGSPEAK_FORM_HTTP_CLEAN_ALL_FEEDS_REQUEST(channel, key)  \
        ("DELETE /channels/" + channel + "/feeds/?api_key=" + key + " HTTP/1.1\nHost: " + THINGSPEAK_HOST + "\r\n\r\n";)

/* Http request example
 * GET /channels/66225/field/field1/last.txt?key=CHO2NZG9CGGVVWPA HTTP/1.1
 * Host: api.thingspeak.com
 */
#define THINGSPEAK_FORM_HTTP_READ_LAST_ENTRY_IN_FIELD_FEED_REQUEST(channel, key, field) \
        ("GET /channels/" + channel +"/field/" + field + "/last.txt?key=" + key + " HTTP/1.1\nHost: " + THINGSPEAK_HOST + "\r\n\r\n")

#if DEBUG_SERIAL
  #define DEBUG_PO(msg)     Serial.print(msg)
  #define DEBUG_POLN(msg)   Serial.println(msg)
#else
  #define DEBUG_PO(msg)     {}
  #define DEBUG_POLN(msg)   {}
#endif

WiFiClient client;

/************************************************************
 *  Function: 
 *    Establish connection to api.thingspeak.com
 ************************************************************/
void thingspeak_connect(void)
{
  DEBUG_PO("Connecting to ");
  DEBUG_POLN(THINGSPEAK_HOST);

  while (!client.connect(THINGSPEAK_HOST, 80))
  {
    DEBUG_POLN("Connect FAIL! Retry ...");
    delay(100);
    // TODO : do something here if stuck too long
  }

  client.flush();
}

/************************************************************
 *  Function: 
 *    Disconnect from api.thingspeak.com
 ************************************************************/
void thingspeak_disconnect(void)
{
  if (client.connected())
  {
    client.stop();
    client.flush();
  }
  DEBUG_PO("Disconnected from ");
  DEBUG_POLN(THINGSPEAK_HOST);
}

/************************************************************
 *  Function: 
 *    Get the last value in the field feed of 
 *    selected channel from api.thingspeak.com
 ************************************************************/
int thingspeak_getLastValueFieldFeed(String channel, String key, String field_name)
{
  String http_code = THINGSPEAK_FORM_HTTP_READ_LAST_ENTRY_IN_FIELD_FEED_REQUEST(channel, key, field_name);
  String line;
  int field_value = 0;
  
  thingspeak_connect();

  // DEBUG_POLN(http_code);
  client.print(http_code);

  delay(50);
  while (!client.available());

  // First return line format "HTTP-Version Status-Code Reason-Phrase\r\n"
  // Ignore http version
  client.readStringUntil(' ');
  // Read the return code
  if (client.readStringUntil(' ').toInt() != 200)
  {
    return -1;    // Something went wrong. Return 0xFFFF for error reading
  }

  // Ignore the rest info of header in http response
  // Read until get the empty line -> End of header
  while (!client.readStringUntil('\n').equals("\r"));

  // Ignore the line denote for the raw length of body
  client.readStringUntil('\n');

  field_value = client.readStringUntil('\n').toInt();

  thingspeak_disconnect();

  return field_value;
}

void setup() 
{
  #if DEBUG_SERIAL
  // Start serial
  Serial.begin(115200);
  #endif

  delay(1000);

  DEBUG_POLN("\n\n====== ESP8266 Module Info ======");
  DEBUG_PO("Chip ID            : ");
  DEBUG_POLN(ESP.getChipId());
  DEBUG_PO("Flash size (bytes) : ");
  DEBUG_POLN(ESP.getFlashChipSize());
  DEBUG_PO("Free heap (bytes)  : ");
  DEBUG_POLN(ESP.getFreeHeap());
  DEBUG_POLN("=================================\n");

  // Start wifi
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  DEBUG_POLN("\n==============================");
  DEBUG_PO("Connecting to ");
  DEBUG_PO(WIFI_SSID);

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    DEBUG_PO(".");
  }

  DEBUG_PO("\nConnected. IP address: ");
  DEBUG_POLN(WiFi.localIP());
}

void loop() 
{
  static int counter = 5;
  
  if (counter != 0)
  {
    DEBUG_POLN(thingspeak_getLastValueFieldFeed(THINGSPEAK_LIGHT_NO1_CHANNEL, THINGSPEAK_LIGHT_NO1_READ_KEY, "field1"));
    counter--;
  }

  delay(1000);
}

