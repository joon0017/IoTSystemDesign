#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <SPI.h>

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
#define DHT_PIN D3
#define DHTTYPE DHTesp::DHT22
#define RELAY1_PIN D4
#define RELAY_OFF HIGH
#define RELAY_ON LOW
#define LED_ON HIGH
#define LED_OFF LOW

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void writeString(void) {
  display.clearDisplay();

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  // Not all the characters will fit on the display. This is normal.
  // Library will draw what it can and the rest will be clipped.
  // for(int16_t i=0; i<256; i++) {
  //   if(i == '\n') display.write(' ');
  //   else          display.write(i);
  // }

  char toWrite[15] = "Seong Joon Cho";
  for (int16_t i=0; i<15;i++){
    display.write(toWrite[i]);
    delay(100);
  }

  display.display();
  delay(2000);
}

void writeStringWithStyle(void) {
  display.clearDisplay();

  display.setTextSize(2);             // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_BLACK, SSD1306_WHITE); // Draw 'inverse' text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Seong Joon Cho"));
  display.display();
  delay(2000);
}

void scrollText(void) {
  display.clearDisplay();

  display.setTextSize(2); // Draw 2X-scale text
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("Seong Joon Cho"));
  display.display();      // Show initial text
  delay(100);

  // Scroll in various directions, pausing in-between:
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
  delay(1000);
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();

}

void loop() {
  // put your main code here, to run repeatedly:
  display.clearDisplay();
  writeString();
  delay(2000);
  
  display.clearDisplay();
  writeStringWithStyle();
  delay(2000);

  display.clearDisplay();
  scrollText();
  delay(2000);
}
