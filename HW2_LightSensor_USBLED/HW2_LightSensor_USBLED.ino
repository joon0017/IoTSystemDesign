#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <DHTesp.h>

#define D0 16
#define D1 5
#define D2 4
#define D3 0
#define D4 2
#define D5 14
#define D6 12
#define D7 13
#define D8 15

#define LIGHT_PIN A0  // 조도센서 연결핀
#define LED_PIN D0 
#define RELAY1_PIN D4
#define RELAY_OFF HIGH
#define RELAY_ON LOW
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

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int lightValue;
int relay_state = RELAY_OFF;
int usable = 1;

unsigned long lightTimer;
unsigned long tempTimer;
unsigned long currentTime;

float humidity, temperature;
DHTesp dht;

void displayDHT();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(10);
  pinMode(LED_PIN,OUTPUT); //initialize led as output
  digitalWrite(LED_PIN,LOW); //led is off
  pinMode(LIGHT_PIN, INPUT); //light sensor input
  pinMode(RELAY1_PIN,OUTPUT); //relay signals
  digitalWrite(RELAY1_PIN,RELAY_OFF); //relay off
  //USB LED setup complete

  dht.setup(DHT_PIN,DHTTYPE);
  //dht setup complete

  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  //display setup complete


  usable = 1;
  currentTime = millis();
  lightTimer = currentTime;
  tempTimer = currentTime;
  //Asynchronous timer setup complete
}

void loop() {
  // put your main code here, to run repeatedly:
  currentTime = millis();
  lightValue = analogRead(LIGHT_PIN);
  Serial.print("Timer difference: ");
  Serial.println(currentTime - lightTimer);
  if (currentTime - tempTimer > 1000) {
    displayDHT();
    //change light value part so it displays temp value on oled
    tempTimer = currentTime;
  }

  if((relay_state != RELAY_ON && lightValue < 500) && usable == 1)
  {
    relay_state = RELAY_ON;
    lightTimer = currentTime;
    usable = 0;
  }
  else if(currentTime - lightTimer > 10000)
  {
    relay_state = RELAY_OFF;
    if(lightValue > 700)
      usable = 1;
  }

  digitalWrite(RELAY1_PIN,relay_state);

}

void displayDHT(){
  humidity = dht.getHumidity();
  temperature = dht.getTemperature();

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
