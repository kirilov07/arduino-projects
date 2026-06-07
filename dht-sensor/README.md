# DHT22 — Temperature & Humidity Sensor

Reads ambient temperature (°C / °F) and relative humidity (%) using a single-wire digital protocol. The DHT22 is more accurate and has a wider range than the DHT11.

---

## Demo

<!-- Add your photo or video here -->
> 📷 _Drop a photo or video into `assets/` and link it here_

---

## Components

| Component | Quantity |
|-----------|----------|
| Arduino Uno / Mega | 1 |
| DHT22 Sensor | 1 |
| 10kΩ resistor (pull-up) | 1 |
| Breadboard + jumper wires | — |

**Library required:** DHT sensor library by Adafruit
→ Arduino IDE: **Sketch → Include Library → Manage Libraries → search "DHT sensor library"**

---

## Wiring

| DHT22 Pin | Arduino Pin |
|-----------|-------------|
| VCC (pin 1) | 5V |
| DATA (pin 2) | Pin 2 + 10kΩ pull-up to 5V |
| GND (pin 4) | GND |

---

## How it works

The DHT22 uses a proprietary single-wire protocol. When triggered, it sends 40 bits of data: 16 bits humidity, 16 bits temperature, 8 bits checksum. The library handles the timing and decoding.

- Temperature range: −40°C to +80°C (±0.5°C accuracy)
- Humidity range: 0–100% RH (±2–5% accuracy)
- Sampling rate: max once every 2 seconds

---

## Code

```cpp
#include <DHT.h>

#define DHT_PIN  2
#define DHT_TYPE DHT22

DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
  Serial.println("DHT22 ready");
}

void loop() {
  delay(2000); // DHT22 needs 2s between reads

  float humidity    = dht.readHumidity();
  float tempC       = dht.readTemperature();
  float tempF       = dht.readTemperature(true);

  if (isnan(humidity) || isnan(tempC)) {
    Serial.println("Sensor read failed");
    return;
  }

  Serial.print("Humidity:    "); Serial.print(humidity);   Serial.println(" %");
  Serial.print("Temperature: "); Serial.print(tempC);      Serial.println(" °C");
  Serial.print("             "); Serial.print(tempF);      Serial.println(" °F");
  Serial.println("---");
}
```

---

## How to run

1. Install the **DHT sensor library** (Adafruit) via Library Manager.
2. Wire as shown above — the pull-up resistor on the DATA line is required.
3. Upload `code.ino`, open Serial Monitor at **9600 baud**.

---

## Expected output

```
DHT22 ready
Humidity:    52.30 %
Temperature: 23.50 °C
             74.30 °F
---
```
