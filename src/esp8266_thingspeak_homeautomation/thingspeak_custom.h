#ifndef __THINGSPEAK_CUSTOM_H__
#define __THINGSPEAK_CUSTOM_H__

#include <Arduino.h>
#include <Client.h>

namespace thingspeak_custom
{
  class ThingSpeak
  {
  public:
    ThingSpeak(Client *client,
               String channel = "",
               String read_key = "",
               String write_key = "",
               String talkback_id = "",
               String talkback_key = "");
    virtual ~ThingSpeak(void);
    
    void SetChannelInfo(String channel,
                        String read_key,
                        String write_key,
                        String talkback_id,
                        String talkback_key);
    int GetLastValueFieldFeed(String field_name);
    String GetTalkBackCmd(void);
    
  private:
    String channel_str;
    String read_key_str;
    String write_key_str;
    
    String talkback_id_str;
    String talkback_key_str;
    String talkback_http_post;
    Client *client;
    
    void Connect(void);
    void Disconnect(void);
    int SendHttpRequest(String msg); 
    int ReadHeader(void);
    String ReadBody(void);
  };
}

#endif  // __THINGSPEAK_CUSTOM_H__

