/* design1_mqtt.ino */

#include "EspMQTTClient.h"
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <DHTesp.h>
#include <ArduinoJson.h>

#define D0 16
#define D1 5
#define D2 4
#define D3 0
#define D4 2
#define D5 14
#define D6 12
#define D7 13
#define D8 15

#define LIGHT_PIN A0
#define LED_PIN D0
#define RELAY1_PIN D4
#define CDS_PIN A0
#define RELAY_OFF HIGH //HIGH signal should disable relay
#define RELAY_ON LOW //LOW signal should enable relay
#define LED_ON HIGH
#define LED_OFF LOW

//DHT related variables
#define DHT_PIN D3
#define DHTTYPE DHTesp::DHT22

//OLED variables
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C

//EspMQTTClient.h settings
#define  MQTT_CLIENT_NAME "21900699@HGU"
const char*  WLANSSID = "SJ";
const char*  WLANPW = "tjdwns1111";
#define MQTTBROKER "sweetdream.iptime.org"  // class
// #define MQTTBROKER "192.168.117.71"  // personal
#define MQTTUNAME "iot"
#define MQTTPW "csee1414"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

EspMQTTClient client(
  WLANSSID,
  WLANPW,
  MQTTBROKER,  // MQTT Broker server ip
  MQTTUNAME,   // Can be omitted if not needed
  MQTTPW,   // Can be omitted if not needed
  MQTT_CLIENT_NAME,     // Client name that uniquely identify your device
  1883              // The MQTT port, default to 1883. this line can be omitted
);

const char* mqtt_topic = "iot/21900699";
char pub_data[256];

int lightValue;
int relay_state = RELAY_OFF;
int usbControlled = 0;
int event_usable = 0;
int eventTrig = 0;

unsigned long currentTime;
unsigned long cdsTimer;
unsigned long light_Timer;
unsigned long DHT_Timer;
unsigned long mqttTimer;
const long DHT_interval = 1000;

float humidity, temperature;
DHTesp dht;

void displayDHT();
void autoLight();
void sendJson();
void sendMqtt();
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  delay(10);

  pinMode(LED_PIN,OUTPUT); //initialize led as output
  digitalWrite(LED_PIN,LOW); //led is off
  pinMode(LIGHT_PIN, INPUT); //light sensor input
  pinMode(RELAY1_PIN,OUTPUT); //relay signals
  digitalWrite(RELAY1_PIN,RELAY_OFF); //relay off

  dht.setup(DHT_PIN,DHTTYPE);

  
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.display();
  delay(2000);
  display.clearDisplay();
  //display setup complete

  currentTime = millis();
  light_Timer = currentTime;
  DHT_Timer = currentTime;
  cdsTimer = currentTime;
  mqttTimer = currentTime;
  event_usable = 0;
}

// This function is called once everything is connected (Wifi and MQTT)
// WARNING : YOU MUST IMPLEMENT IT IF YOU USE EspMQTTClient
void onConnectionEstablished()
{
  // Subscribe to "mytopic/test" and display received message to Serial
  client.subscribe(mqtt_topic, [](const String & payload) {
    Serial.println(payload);
    
    //led
    if(payload == "ledoff"){
      digitalWrite(LED_PIN, LED_OFF);
    }
    else if(payload == "led") {
      int led_state = digitalRead(LED_PIN);
      digitalWrite(LED_PIN, !led_state);
      Serial.println("Switching LED");
    }
    else if (payload == "ledon") {
      digitalWrite(LED_PIN,LED_ON);
    }

    //usbled
    else if ((currentTime - light_Timer > 10000) && (!usbControlled && payload == "usbled")) {
      if(relay_state) relay_state = RELAY_ON;
      else relay_state = RELAY_OFF;
      Serial.print("Switching USB LED: current state ");
      Serial.println(relay_state);
    }
    else if (payload == "usbledon"){
      relay_state = RELAY_ON;
      usbControlled = 1;
    }
    else if (payload == "usbledoff"){
      relay_state = RELAY_OFF;
      usbControlled = 0;

      //allow autotimer to be used;
      event_usable = 0;
      light_Timer -= 10000;
    }
    
    //dht and cds
    else if (payload == "dht22") {
      temperature = dht.getTemperature();
      humidity = dht.getHumidity();
      StaticJsonDocument<256> doc;

      doc["humidity"] = humidity;
      doc["temperature"] = temperature;
      
      String output;
      serializeJsonPretty(doc,output);
      client.publish("iot/21900699/dht22",output);
    }    
    else if (payload == "cds"){
      lightValue = analogRead(LIGHT_PIN);
      
      StaticJsonDocument<256> doc;

      doc["cds"] = lightValue;

      String output;
      serializeJsonPretty(doc,output);
      client.publish("iot/21900699/cds",output);
    }
  });
    
  client.subscribe("iot/21900699/wifi", [](const String & payload) {
    Serial.println(payload);
    StaticJsonDocument<200> doc;

    DeserializationError error = deserializeJson(doc,payload);
    if(error){
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    WLANSSID = doc["SSID"];
    WLANPW = doc["PW"];

    Serial.println("\n\nChanges access point information: ");
    Serial.print("SSID: ");
    Serial.println(WLANSSID);
    Serial.print("PW: ");
    Serial.println(WLANPW);
    Serial.println();

    // client.setWifiCredentials(const char *wifiSsid, const char *wifiPassword)
    client.setWifiCredentials(WLANSSID, WLANPW);
    WiFi.disconnect(true);
    MDNS.end();
    });
  // Publish a message
  client.publish("iot/21900699", "successfully connected"); 
}



void loop() {
  // put your main code here, to run repeatedly:
  currentTime = millis();

  sendJson();

  if (!usbControlled) autoLight();

  digitalWrite(RELAY1_PIN,relay_state);
  client.loop();    
}




void sendJson(){
  if (currentTime - DHT_Timer > 10000) {
    Serial.print("\nCurrent event_usable: ");
    Serial.print(event_usable);
    Serial.print("\nCurrent usbControlled: ");
    Serial.println(usbControlled);
    Serial.print("\nCurrent relay_state: ");
    Serial.println(relay_state);
    Serial.println();
    Serial.print("currentTime: ");
    Serial.print(currentTime);
    Serial.print("; DHTtimer: ");
    Serial.println(DHT_Timer);
    
    humidity = dht.getHumidity();
    // Serial.print("humidity: ");
    // Serial.println(humidity);
    
    temperature = dht.getTemperature();
    // Serial.print("temperature: ");
    // Serial.println(temperature);

    lightValue = analogRead(LIGHT_PIN);
    // Serial.print("light value: ");
    // Serial.println(lightValue);

    displayDHT();
    StaticJsonDocument<256> doc;

    doc["humidity"] = humidity;
    doc["temperature"] = temperature;
    doc["cds"] = lightValue;
    // serializeJsonPretty(doc,Serial);
    // Serial.println(); 
    
    String output;
    serializeJsonPretty(doc,output);
    client.publish("iot/21900699/autodht22",output);

    
    DHT_Timer = currentTime;
    // Serial.println("Temperature is %d\nHumidity is %d\nLight itensity is %d",temperature,humidity,lightValue);
  }
}

void sendMqtt(){
  if (currentTime - mqttTimer > 3000) {
    sprintf(pub_data,"%3.2f",temperature);
    client.publish("iot/21900699/dht_t",pub_data);
    sprintf(pub_data,"%3.2f",humidity);
    client.publish("iot/21900699/dht_h",pub_data);
    sprintf(pub_data,"%d",lightValue);
    client.publish("iot/21900699/CDS",pub_data);
    mqttTimer = currentTime;
  }
}

void autoLight(){  
  if (currentTime - cdsTimer > 500){
    lightValue = analogRead(LIGHT_PIN);
    cdsTimer = currentTime;
  }
  if(event_usable && (relay_state == RELAY_OFF && lightValue < 500)){
    eventTrig = 1;
    relay_state = RELAY_ON;
    light_Timer = currentTime;
    event_usable = 0;
    Serial.println("event triggered");
  }
  else if((!event_usable && (currentTime - light_Timer > 10000 )) && lightValue > 600 ) {
    Serial.println("turning off due to event");
    event_usable = 1;
    relay_state = RELAY_OFF;
  }
  else if((eventTrig && lightValue < 600) && (currentTime - light_Timer > 10000 )/*10 seconds*/){
    Serial.println("turning off after 10 seconds");
    event_usable = 0;
    eventTrig = 0;
    relay_state = RELAY_OFF;
  } 
}

void displayDHT(){

  display.clearDisplay();

  display.setTextSize(1.5);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.print("T: ");
  display.print(temperature);
  display.print((char)248); //°
  display.println("C");
  display.print("H: ");
  display.print(humidity);
  display.println("%");

  display.display();
}
