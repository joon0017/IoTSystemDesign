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

#define DHTPIN D3
#define DHTTYPE DHTesp::DHT22

float humidity, temperature;
DHTesp dht;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  dht.setup(DHTPIN,DHTTYPE);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(dht.getMinimumSamplingPeriod());
  humidity = dht.getHumidity();
  temperature = dht.getTemperature();

  if( !isnan(humidity) && !isnan(temperature)){
    Serial.print("Temperature = ");
    Serial.print(temperature);
    Serial.print("\t");
    Serial.print("Humidity = ");
    Serial.println(humidity);
  } else {
    Serial.println("Error while reading from DHT sensor");
  }
  delay(2000);

}
