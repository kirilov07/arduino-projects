// DHT22 Temperature & Humidity Sensor
// Requires: DHT sensor library by Adafruit
// Wiring: VCC→5V, GND→GND, DATA→Pin2 (+ 10kΩ pull-up to 5V)

#include <DHT.h>

#define DHT_PIN  2
#define DHT_TYPE DHT22

DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
  Serial.println("DHT22 Temperature & Humidity Sensor Ready");
  Serial.println("------------------------------------------");
}

void loop() {
  delay(2000); // Minimum 2s between reads

  float humidity = dht.readHumidity();
  float tempC    = dht.readTemperature();
  float tempF    = dht.readTemperature(true);

  if (isnan(humidity) || isnan(tempC)) {
    Serial.println("ERROR: Failed to read from DHT22 sensor");
    return;
  }

  // Heat index (feels-like temperature)
  float heatIndexC = dht.computeHeatIndex(tempC, humidity, false);
  float heatIndexF = dht.computeHeatIndex(tempF, humidity);

  Serial.print("Humidity:    "); Serial.print(humidity, 1);    Serial.println(" %");
  Serial.print("Temperature: "); Serial.print(tempC, 1);       Serial.print(" °C  /  ");
                                  Serial.print(tempF, 1);       Serial.println(" °F");
  Serial.print("Heat index:  "); Serial.print(heatIndexC, 1);  Serial.print(" °C  /  ");
                                  Serial.print(heatIndexF, 1);  Serial.println(" °F");
  Serial.println("---");
}
