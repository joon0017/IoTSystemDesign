#define D0 16
#define D4 2
#define LIGHT_PIN A0
#define LED_PIN D0
#define RELAY1_PIN D4
#define RELAY_OFF HIGH //HIGH signal should disable relay
#define RELAY_ON LOW //LOW signal should enable relay
int lightValue;
int relay_state = RELAY_OFF;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  delay(10);
  pinMode(LED_PIN,OUTPUT); //initialize led as output
  digitalWrite(LED_PIN,LOW); //led is off
  pinMode(LIGHT_PIN, INPUT); //light sensor input
  pinMode(RELAY1_PIN,OUTPUT); //relay signals
  digitalWrite(RELAY1_PIN,RELAY_OFF); //relay off

}

void loop() {
  // put your main code here, to run repeatedly:
  lightValue = analogRead(LIGHT_PIN);

  if(lightValue < 500){
    relay_state = RELAY_ON;
  }
  else if (lightValue > 700){
    relay_state = RELAY_OFF;
  }
  digitalWrite(RELAY1_PIN,relay_state);
  Serial.print("Current light value: ");
  Serial.println(lightValue);
  delay(500);
}
