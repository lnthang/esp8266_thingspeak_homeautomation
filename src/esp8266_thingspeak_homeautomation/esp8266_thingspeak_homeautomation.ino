#include <ESP8266WiFi.h>
#include "thingspeak_custom.h"

#define COMP          0
#define HOME          1
#define DEBUG_SERIAL  1

#if COMP
  #define WIFI_SSID   "MISFIT SW"
  #define WIFI_PASS   "Wearables1!"
#elif HOME
  #define WIFI_SSID   "Hong_quan"
  #define WIFI_PASS   "0908108060a"
#endif
  
#define THINGSPEAK_LIGHT_NO1_CHANNEL        "66225"
#define THINGSPEAK_LIGHT_NO1_READ_KEY       "X1ZDQFFMYXEVQB7G"

#define THINGSPEAK_LIGHT_NO1_LOG_CHANNEL    "66636"
#define THINGSPEAK_LIGHT_NO1_LOG_WRITE_KEY  "V65MD3S4STAVNEUB"

#if DEBUG_SERIAL
  #define DEBUG_PO(msg)     Serial.print(msg)
  #define DEBUG_POLN(msg)   Serial.println(msg)
#else
  #define DEBUG_PO(msg)     {}
  #define DEBUG_POLN(msg)   {}
#endif

using namespace thingspeak_custom;

WiFiClient client;
ThingSpeak ts_light_no1(&client);

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

  ts_light_no1.SetChannelInfo(THINGSPEAK_LIGHT_NO1_CHANNEL, THINGSPEAK_LIGHT_NO1_READ_KEY, "");
}

void loop() 
{
  static int counter = 5;
  
  if (counter != 0)
  {
    counter--;
    DEBUG_POLN(ts_light_no1.GetLastValueFieldFeed("field1"));
  }

  delay(1000);
}

