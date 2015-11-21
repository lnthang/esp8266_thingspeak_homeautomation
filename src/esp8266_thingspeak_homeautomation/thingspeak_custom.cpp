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

/* Http request example
 * POST /talkbacks/4059/commands/execute?api_key=CIN3LKFSII32LUYN HTTP/1.1
 * Host: api.thingspeak.com
 */
#define THINGSPEAK_HTTP_POST_GET_TALKBACK_COMMAND_REQUEST(talkback_id, key) \
        ("POST /talkbacks/" + talkback_id + "/commands/execute?api_key=" + key + " HTTP/1.1\nHost: " + THINGSPEAK_HOST + "\r\n\r\n")


namespace thingspeak_custom {
  ThingSpeak::ThingSpeak(Client *client, String channel, String read_key, String write_key, String talkback_id, String talkback_key)
  {
    this->client = client;

    SetChannelInfo(channel, read_key, write_key, talkback_id, talkback_key);
  }

  ThingSpeak::~ThingSpeak(void)
  {

  }

  /************************************************************
   *  Function: 
   *    Set Channel info
   ************************************************************/
  void ThingSpeak::SetChannelInfo(String channel, String read_key, String write_key, String talkback_id, String talkback_key)
  {
    this->channel_str = channel;
    this->read_key_str = read_key;
    this->write_key_str = write_key;

    this->talkback_id_str = talkback_id;
    this->talkback_key_str = talkback_key;
    this->talkback_http_post = THINGSPEAK_HTTP_POST_GET_TALKBACK_COMMAND_REQUEST(talkback_id, talkback_key);
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
    
    Connect();
    this->client->print(http_code);
    THINGSPEAK_DEBUGLN("\nSent 'read last entry in field feed' request.\nWaiting response from server");
    
    while (!this->client->available())
    {
      THINGSPEAK_DEBUG(".");
    }
    THINGSPEAK_DEBUGLN("");

    // Read the return code
    if (ReadHeader() != 200) {
      THINGSPEAK_DEBUGLN("Reading data failed");
      field_value = -1; // Something went wrong. Return 0xFFFF for error reading
    } else {
      field_value = ReadBody().toInt();
    }

    Disconnect();

    return field_value;
  }

  /************************************************************
   *  Function: 
   *    Get TalkBack command if any from channel.
   ************************************************************/
  String ThingSpeak::GetTalkBackCmd(void)
  {
    String talk_back_cmd = "";
    
    Connect();

    // this->client->print(this->talkback_http_post);
    // THINGSPEAK_DEBUGLN("\n-> Polling for TalkBack command.\nWaiting response from server");

    // delay(10);

    // while (!this->client->available())
    // {
      // Serial.print("!");
    // }
    // THINGSPEAK_DEBUGLN("");
    if (SendHttpRequest(this->talkback_http_post) != 0)
    { // Something wrong happened
      Disconnect();
      Serial.println("Send POST request failed.");
      return "-1";
    }
    
    // Read the return code
    if (ReadHeader() != 200) {
      THINGSPEAK_DEBUGLN("Reading data failed");
      talk_back_cmd = "-1";    // Something went wrong. Return 0xFFFF for error reading
    } else {
      talk_back_cmd = ReadBody();
    }
    
    Disconnect();
    
    return talk_back_cmd;
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
    THINGSPEAK_DEBUGLN("");

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
    THINGSPEAK_DEBUGLN("Disconnected from to api.thingspeak.com");
  }

  int ThingSpeak::SendHttpRequest(String msg)
  {
    int status_code = -1;  // 0 ~ Success, -1 ~ Timeout, -2 Unknown failure
    int timeout = 10000;

    this->client->print(msg);
    while(timeout--)
    {
      if (this->client->available())
      {
        status_code = 0;
        break;
      }
    }

    return status_code;
  }

  int ThingSpeak::ReadHeader(void)
  {
    int header_status_code = 0;
    int halt_counter = 0;

    // "HTTP-Version Status-Code Reason-Phrase\r\n"
    // Ignore http version
    this->client->readStringUntil(' ');

    // Read the return code
    header_status_code = this->client->readStringUntil(' ').toInt();

    // Ignore the rest info of header in http response
    // Read until get the empty line -> End of header
    while (!this->client->readStringUntil('\n').equals("\r"))
    {
      halt_counter++;
      if (halt_counter >= 5000)
      {
        header_status_code = -1;
        break;
      }
    }

    return header_status_code;
  }
  
  String ThingSpeak::ReadBody(void)
  {
    String body = "";
    // Ignore the line denote for the raw length of body
    this->client->readStringUntil('\n');
    
    body = this->client->readStringUntil('\n');
    
    return body;
  }
}
