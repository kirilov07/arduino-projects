// Environmental Monitor — DHT22 + Alert LEDs
// Library: DHT sensor library by Adafruit
// Wiring: DATA→Pin2 (+10kΩ pull-up), RedLED→Pin7, BlueLED→Pin8

#include <DHT.h>

#define DHT_PIN    2
#define DHT_TYPE   DHT22
#define LED_TEMP   7    // Red  — temperature alert
#define LED_HUM    8    // Blue — humidity alert

const float TEMP_MAX = 30.0;  // °C threshold
const float HUM_MAX  = 80.0;  // %  threshold

DHT dht(DHT_PIN, DHT_TYPE);

void printDashboard(float hum, float tempC, float tempF, float hi) {
  Serial.println("===========================");
  Serial.print("Temp:      "); Serial.print(tempC, 1); Serial.print(" C  /  "); Serial.print(tempF, 1); Serial.println(" F");
  Serial.print("Humidity:  "); Serial.print(hum, 1); Serial.println(" %");
  Serial.print("Heat idx:  "); Serial.print(hi, 1); Serial.println(" C");
  Serial.print("Status:    ");
  bool tAlert = tempC > TEMP_MAX;
  bool hAlert = hum   > HUM_MAX;
  if (!tAlert && !hAlert) Serial.println("OK");
  else {
    if (tAlert) Serial.print("HEAT ALERT  ");
    if (hAlert) Serial.print("HUMIDITY ALERT");
    Serial.println();
  }
  Serial.println("===========================");
  Serial.println();
}

void setup() {
  Serial.begin(9600);
  dht.begin();
  pinMode(LED_TEMP, OUTPUT);
  pinMode(LED_HUM,  OUTPUT);
  Serial.println("Environmental Monitor Ready");
  Serial.println();
}

void loop() {
  delay(2000); // DHT22 minimum 2s between reads

  float hum   = dht.readHumidity();
  float tempC = dht.readTemperature();
  float tempF = dht.readTemperature(true);

  if (isnan(hum) || isnan(tempC)) {
    Serial.println("ERROR: Sensor read failed. Check wiring and pull-up resistor.");
    return;
  }

  float hi = dht.computeHeatIndex(tempC, hum, false);

  digitalWrite(LED_TEMP, tempC > TEMP_MAX);
  digitalWrite(LED_HUM,  hum   > HUM_MAX);

  printDashboard(hum, tempC, tempF, hi);
}
