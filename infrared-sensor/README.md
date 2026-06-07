# Infrared (IR) Obstacle Sensor

Detects objects in front of the sensor by emitting infrared light and measuring the reflection. Used for obstacle avoidance, proximity detection, and line-following robots.

---

## Demo

<!-- Add your photo or video here -->
> 📷 _Drop a photo or video into `assets/` and link it here_

---

## Components

| Component | Quantity |
|-----------|----------|
| Arduino Uno / Mega | 1 |
| IR Obstacle Sensor Module | 1 |
| LED (optional indicator) | 1 |
| Jumper wires | 3 |

---

## Wiring

| IR Module Pin | Arduino |
|--------------|---------|
| VCC | 5V |
| GND | GND |
| OUT | Pin 8 |

The module has a small potentiometer to adjust detection distance (typically 2–30 cm).

---

## How it works

The module has an IR LED emitter and a photodiode receiver. When infrared light bounces off a nearby object and hits the receiver:
- **OUT = LOW** → object detected (active-LOW)
- **OUT = HIGH** → no object in range

The sensitivity (detection range) is tuned with the onboard potentiometer.

---

## Code

```cpp
const int IR_PIN  = 8;
const int LED_PIN = 13;

void setup() {
  Serial.begin(9600);
  pinMode(IR_PIN,  INPUT);
  pinMode(LED_PIN, OUTPUT);
  Serial.println("IR Obstacle Sensor Ready");
}

void loop() {
  int state = digitalRead(IR_PIN);

  if (state == LOW) {  // Active-LOW: LOW = obstacle detected
    digitalWrite(LED_PIN, HIGH);
    Serial.println("OBSTACLE DETECTED");
  } else {
    digitalWrite(LED_PIN, LOW);
    Serial.println("Clear");
  }

  delay(200);
}
```

---

## How to run

1. Wire as shown above.
2. Upload `code.ino`, open Serial Monitor at **9600 baud**.
3. Move your hand toward the sensor — you'll see "OBSTACLE DETECTED" and the LED will light up.
4. Adjust the potentiometer on the sensor module to change detection distance.
