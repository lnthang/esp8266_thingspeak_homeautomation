#ifndef __THINGSPEAK_CUSTOM_H__
#define __THINGSPEAK_CUSTOM_H__

#include <WiFiClient.h>

namespace thingspeak_custom 
{
  class ThingSpeak 
  {
    public:
      ThingSpeak(WiFiClient *client, String channel = "", String read_key = "", String write_key = "");
      virtual ~ThingSpeak(void);

      void SetChannelInfo(String channel, String read_key, String write_key);
      int GetLastValueFieldFeed(String field_name);

    private:
      String channel_str;
      String read_key_str;
      String write_key_str;
      WiFiClient *client;

      void Connect(void);
      void Disconnect(void);
  };
}

#endif  // __THINGSPEAK_CUSTOM_H__

