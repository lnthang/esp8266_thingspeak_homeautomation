#include <ESP8266WiFi.h>
#include "thingspeak_custom.h"

#define COMP          1
#define HOME          0
#define DEBUG_SERIAL  1

#define D0    16  // GPIO16
#define D1    5   // GPIO5
#define D2    4   // GPIO4
#define D3    0   // GPIO0
#define D4    2   // GPIO2
#define D5    14  // GPIO14
#define D6    12  // GPIO12
#define D7    13  // GPI13
#define D8    15  // GPIOO15
#define D9    3   // GPIO3
#define D10   1   // GPIO1

#define NETWORK_SEL_PIN   D5
#define LED_1   D1
#define LED_2   D2
#define LED_3   D4

//#if COMP
  #define WIFI_SSID_COMP   "MISFIT SW"
  #define WIFI_PASS_COMP   "Wearables1!"
//#elif HOME
  #define WIFI_SSID_HOME   "Hong_quan"
  #define WIFI_PASS_HOME   "0908108060a"
//#endif
  
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

  pinMode(NETWORK_SEL_PIN, INPUT);

  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  digitalWrite(LED_1, 1);
  digitalWrite(LED_2, 1);
  digitalWrite(LED_3, 1);

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
  DEBUG_PO("Connecting to ");
  if (digitalRead(NETWORK_SEL_PIN) == 1) {
    WiFi.begin(WIFI_SSID_COMP, WIFI_PASS_COMP);
    DEBUG_PO(WIFI_SSID_COMP);
  } else {
    WiFi.begin(WIFI_SSID_HOME, WIFI_PASS_HOME);
    DEBUG_PO(WIFI_SSID_HOME);
  }

  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    DEBUG_PO(".");
  }

  DEBUG_PO("\nConnected. IP address: ");
  DEBUG_POLN(WiFi.localIP());

  ts_light_no1.SetChannelInfo(THINGSPEAK_LIGHT_NO1_CHANNEL, THINGSPEAK_LIGHT_NO1_READ_KEY, "F9I3HN9MA22DOE1L", "4059", "CIN3LKFSII32LUYN");

  DEBUG_PO("\nRunning...");
}

void loop() 
{
  String talkback_cmd = "";
  talkback_cmd = ts_light_no1.GetTalkBackCmd();

  // Toggle and LED to indicate that it is still running
  digitalWrite(LED_3, !digitalRead(LED_3));

  if (talkback_cmd.equals("\r")) {
    // DEBUG_POLN("No command");
  } else if (talkback_cmd.equals("LED_1_ON\r")) {
    digitalWrite(LED_1, 0);
  } else if (talkback_cmd.equals("LED_1_OFF\r")) {
    digitalWrite(LED_1, 1);
  } else if (talkback_cmd.equals("LED_2_ON\r")) {
    digitalWrite(LED_2, 0);
  } else if (talkback_cmd.equals("LED_2_OFF\r")) {
    digitalWrite(LED_2, 1);
  }

  delay(200);
}

