#define LIGHT_PIN A0

int light_val;

void setup() {
  pinMode(LIGHT_PIN,INPUT);
  Serial.begin(9600);
}

void loop() {
  light_val = analogRead(LIGHT_PIN);
  Serial.print("Light intensity = ");
  Serial.println(light_val);
  delay(1000);
}
