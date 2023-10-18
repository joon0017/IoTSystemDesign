/*
lab7_rpi_flask_NodeMCU_LED_HDT_MQTT.ino
*/      
#include "EspMQTTClient.h"
#include <DHTesp.h>

#define D0 16
#define D4 2 //usbled
#define D5 14
#define D6 12
#define D7 13
#define D8 15

//actuators and sensors
#define LED_PIN D0
#define DHT_PIN D3
#define USDLED_PIN D4
#define CDS_PIN A0

//define the signals for controlling devices
#define DHTTYPE DHTesp::DHT22
#define LED_ON HIGH
#define LED_OFF LOW
#define RELAY_OFF HIGH //HIGH signal should disable relay
#define RELAY_ON LOW //LOW signal should enable relay


const char* mqtt_topic = "iot/21900699";

DHTesp dht;
int lightValue;
float temperature,humidity;
// String msgString;
char pub_data[200];

unsigned long previousMillis_DHT = 0;
const long interval_DHT = 1000;

#define WLANSSID "SJ"
#define WLANPW "tjdwns1111"
// #define MQTTBROKER "192.168.117.71"  // personal
// #define MQTTBROKER "203.252.106.154"  // class
#define MQTTBROKER "sweetdream.iptime.org" //class 2
#define MQTTUNAME "iot"
#define MQTTPW "csee1414"
#define MQTT_CLIENT_NAME "21900699_client"


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

  pinMode(LED_PIN,OUTPUT);
  digitalWrite(LED_PIN, LED_OFF);

  dht.setup(DHT_PIN,DHTesp::DHT22);

}

// This function is called once everything is connected (Wifi and MQTT)
// WARNING : YOU MUST IMPLEMENT IT IF YOU USE EspMQTTClient
void onConnectionEstablished()
{
  // Subscribe to "mytopic/test" and display received message to Serial
  client.subscribe(mqtt_topic, [](const String & payload) {
    Serial.println(payload);

    if (payload == "led") {
      int led_state = digitalRead(LED_PIN);
      digitalWrite(LED_PIN, !led_state);
      Serial.println("Switching LED");
    }

    else if (payload == "ledon") digitalWrite(LED_PIN,LED_ON);
    else if (payload == "ledoff") digitalWrite(LED_PIN, LED_OFF);
    else if (payload == "dht22") {
      temperature = dht.getTemperature();
      humidity = dht.getHumidity();
      sprintf(pub_data,"Temperature is %3.2f, and Humidity is %3.2f",temperature,humidity);
      client.publish("iot/21900699/dht22",pub_data);
    }
    else if (payload == "cds"){
      lightValue = analogRead(CDS_PIN);
      sprintf(pub_data,"Light intensity is %d",lightValue);
      client.publish("iot/21900699/cds",pub_data);
    }
  });

  // // Subscribe to "mytopic/wildcardtest/#" and display received message to Serial
  // client.subscribe("mytopic/wildcardtest/#", [](const String & topic, const String & payload) {
  //   Serial.println("(From wildcard) topic: " + topic + ", payload: " + payload);
  // });

  // Publish a message to "mytopic/test"
  client.publish("iot/21900699", "test 21900699"); // You can activate the retain flag by setting the third parameter to true

  // // Execute delayed instructions
  // client.executeDelayed(5 * 1000, []() {
  //   client.publish("mytopic/wildcardtest/test123", "This is a message sent 5 seconds later");
  // });
}

void loop()
{
  client.loop();
  unsigned long currentMillis_DHT = millis();
  
  //sense every second
  if(currentMillis_DHT - previousMillis_DHT >= interval_DHT ){
    previousMillis_DHT = currentMillis_DHT;
    temperature  = dht.getTemperature();
    humidity = dht.getHumidity();
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.print("\tHumidity: ");
    Serial.println(humidity);
  }
}
