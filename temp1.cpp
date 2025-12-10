#include <DHT.h>

#define DHTPIN A0
#define DHTTYPE DHT11   // or DHT22 if you have one

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  delay(500);
  Serial.println("DHT11 Humidity & Temperature Sensor");
  dht.begin();
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }


  Serial.println(t);



  delay(2000);
}