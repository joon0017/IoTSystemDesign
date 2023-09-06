#define D0 16
#define D1 5
#define D2 4
#define D3 0
#define D4 2
#define D5 14
#define D6 12
#define D7 13
#define D8 15

void setup() {
  // put your setup code here, to run once:
  pinMode(D0, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(D0, HIGH);
  delay(1000);
  digitalWrite(D0, LOW);
  delay(1000);
}
