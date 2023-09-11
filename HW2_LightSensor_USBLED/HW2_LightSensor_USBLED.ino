#define D0 16
#define D4 2

#define LIGHT_PIN A0  // 조도센서 연결핀
#define LED_PIN D0 
#define RELAY1_PIN D4
#define RELAY_OFF HIGH
#define RELAY_ON LOW
#define LED_ON HIGH
#define LED_OFF LOW

int lightValue;
int relay_state = RELAY_OFF;
int usable = 1;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(10);
  pinMode(LED_PIN,OUTPUT); //initialize led as output
  digitalWrite(LED_PIN,LOW); //led is off
  pinMode(LIGHT_PIN, INPUT); //light sensor input
  pinMode(RELAY1_PIN,OUTPUT); //relay signals
  digitalWrite(RELAY1_PIN,RELAY_OFF); //relay off
  usable = 1;

}

void loop() {
  // put your main code here, to run repeatedly:
  lightValue = analogRead(LIGHT_PIN);

  if(usable == 1 && lightValue < 500) {
    relay_state = RELAY_ON;
  }
  else if (usable == 0 && lightValue > 700) usable = 1;

  if (relay_state == RELAY_ON) {
    usable = 0;
    digitalWrite(RELAY1_PIN,relay_state);
    delay(10000);
    relay_state = RELAY_OFF;
    digitalWrite(RELAY1_PIN,relay_state);
  }

  delay(200);
}
