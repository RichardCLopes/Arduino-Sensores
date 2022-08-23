#include "DHT.h"
#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  // Wait a few seconds between measurements.
  delay(3000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float umidade = dht.readHumidity();
  float temp = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(umidade) || isnan(temp)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  Serial.print(F("Umidadde: "));
  Serial.print(umidade);
  Serial.print(F("%  Temperatura: "));
  Serial.print(temp);
  Serial.println(F("°C "));
  
}
