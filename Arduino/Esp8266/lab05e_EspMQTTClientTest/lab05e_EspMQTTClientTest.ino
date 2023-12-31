/*
lab05e_EspMQTTClientTest.ino
*/
#include "EspMQTTClient.h"

#define WLANSSID "SJ"
#define WLANPW "tjdwns1111"
#define MQTTBROKER "203.252.106.154"  // class
// #define MQTTBROKER "192.168.117.140"  // personal
#define MQTTUNAME "iot"
#define MQTTPW "csee1414"
#define MQTT_CLIENT_NAME "sqpNode@HGU"


EspMQTTClient client(
  WLANSSID,
  WLANPW,
  MQTTBROKER,  // MQTT Broker server ip
  MQTTUNAME,   // Can be omitted if not needed
  MQTTPW,   // Can be omitted if not needed
  MQTT_CLIENT_NAME,     // Client name that uniquely identify your device
  1883              // The MQTT port, default to 1883. this line can be omitted
);

void setup()
{
  Serial.begin(115200);

  // // Optional functionalities of EspMQTTClient
  // client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  // client.enableHTTPWebUpdater(); // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overridded with enableHTTPWebUpdater("user", "password").
  // client.enableOTA(); // Enable OTA (Over The Air) updates. Password defaults to MQTTPassword. Port is the default OTA port. Can be overridden with enableOTA("password", port).
  // client.enableLastWillMessage("TestClient/lastwill", "I am going offline");  // You can activate the retain flag by setting the third parameter to true
}

// This function is called once everything is connected (Wifi and MQTT)
// WARNING : YOU MUST IMPLEMENT IT IF YOU USE EspMQTTClient
void onConnectionEstablished()
{
  // Subscribe to "mytopic/test" and display received message to Serial
  client.subscribe("mytopic/test", [](const String & payload) {
    Serial.println(payload);
  });

  // // Subscribe to "mytopic/wildcardtest/#" and display received message to Serial
  // client.subscribe("mytopic/wildcardtest/#", [](const String & topic, const String & payload) {
  //   Serial.println("(From wildcard) topic: " + topic + ", payload: " + payload);
  // });

  // Publish a message to "mytopic/test"
  client.publish("mytopic/test", "tst"); // You can activate the retain flag by setting the third parameter to true

  // // Execute delayed instructions
  // client.executeDelayed(5 * 1000, []() {
  //   client.publish("mytopic/wildcardtest/test123", "This is a message sent 5 seconds later");
  // });
}

void loop()
{
  client.loop();
}
