#include "thingspeak_custom.h"

#if 0
  #define THINGSPEAK_DEBUG(msg)     Serial.print(msg)
  #define THINGSPEAK_DEBUGLN(msg)   Serial.println(msg)
#else
  #define THINGSPEAK_DEBUG(msg)     {}
  #define THINGSPEAK_DEBUGLN(msg)   {}
#endif

#define THINGSPEAK_HOST                     "api.thingspeak.com"

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


namespace thingspeak_custom {
  ThingSpeak::ThingSpeak(WiFiClient *client, String channel, String read_key, String write_key)
  {
    this->channel_str = channel;
    this->read_key_str = read_key;
    this->write_key_str = write_key;
    this->client = client;
  }

  ThingSpeak::~ThingSpeak(void)
  {

  }

  /************************************************************
   *  Function: 
   *    Set Channel info
   ************************************************************/
  void ThingSpeak::SetChannelInfo(String channel, String read_key, String write_key)
  {
    this->channel_str = channel;
    this->read_key_str = read_key;
    this->write_key_str = write_key;
  }

  /************************************************************
   *  Function: 
   *    Get the last value in the field feed of 
   *    selected channel from api.thingspeak.com
   ************************************************************/
  int ThingSpeak::GetLastValueFieldFeed(String field_name)
  {
    String http_code = THINGSPEAK_FORM_HTTP_READ_LAST_ENTRY_IN_FIELD_FEED_REQUEST(this->channel_str, this->read_key_str, field_name);
    int field_value = 0;
    
    this->Connect();
    this->client->print(http_code);

    delay(50);
    while (!this->client->available());

    // First return line format "HTTP-Version Status-Code Reason-Phrase\r\n"
    // Ignore http version
    this->client->readStringUntil(' ');
    // Read the return code
    if (this->client->readStringUntil(' ').toInt() != 200)
    {
      return -1;    // Something went wrong. Return 0xFFFF for error reading
    }

    // Ignore the rest info of header in http response
    // Read until get the empty line -> End of header
    while (!this->client->readStringUntil('\n').equals("\r"));

    // Ignore the line denote for the raw length of body
    this->client->readStringUntil('\n');

    field_value = this->client->readStringUntil('\n').toInt();

    this->Disconnect();

    return field_value;
  }

  /************************************************************
   *  Function: 
   *    Establish connection to api.thingspeak.com
   ************************************************************/
  void ThingSpeak::Connect(void)
  {
    THINGSPEAK_DEBUG("Connecting to api.thingspeak.com");
    while (!this->client->connect(THINGSPEAK_HOST, 80))
    {
      delay(100);
      THINGSPEAK_DEBUG(".");
      // TODO : do something here if stuck too long
    }

    this->client->flush();
  }

  /************************************************************
   *  Function: 
   *    Disconnect from api.thingspeak.com
   ************************************************************/
  void ThingSpeak::Disconnect(void)
  {
    if (this->client->connected())
    {
      this->client->stop();
      this->client->flush();
    }
    THINGSPEAK_DEBUG("Disconnected from to api.thingspeak.com");
  }
}
