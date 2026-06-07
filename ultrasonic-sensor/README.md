# Ultrasonic Sensor — HC-SR04

Distance measurement using ultrasonic sound waves. The sensor emits a pulse and measures how long it takes to bounce back, calculating the distance to the nearest object.

---

## Demo

<!-- Add your photo or video here -->
> 📷 _Add a photo or short video of the circuit working — drop the file into the `assets/` folder and link it below_
<!-- Example: ![Demo](./assets/demo.jpg) -->
<!-- Example: https://github.com/user/repo/assets/demo.mp4 -->

---

## Components

| Component | Quantity |
|-----------|----------|
| Arduino Uno / Mega | 1 |
| HC-SR04 Ultrasonic Sensor | 1 |
| Breadboard | 1 |
| Jumper wires | 4 |

---

## Wiring

| HC-SR04 Pin | Arduino Pin |
|-------------|-------------|
| VCC | 5V |
| GND | GND |
| TRIG | Pin 9 |
| ECHO | Pin 10 |

<!-- Add wiring diagram photo: ![Wiring](./assets/wiring.jpg) -->

---

## How it works

1. Arduino sends a 10µs HIGH pulse on the TRIG pin.
2. HC-SR04 emits 8 ultrasonic bursts at 40kHz.
3. ECHO pin goes HIGH for the duration the sound takes to travel out and return.
4. Distance = (pulse duration × speed of sound) ÷ 2.

Speed of sound at room temperature ≈ 343 m/s = 0.0343 cm/µs.

---

## Code

```cpp
// HC-SR04 Ultrasonic Sensor — Basic Distance Meter
// Prints distance in cm to Serial Monitor

const int TRIG_PIN = 9;
const int ECHO_PIN = 10;

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  // Send trigger pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure echo duration
  long duration = pulseIn(ECHO_PIN, HIGH);

  // Calculate distance in cm
  float distance = duration * 0.0343 / 2.0;

  Serial.print("Distance: ");
  Serial.print(distance, 1);
  Serial.println(" cm");

  delay(500);
}
```

---

## How to run

1. Wire the circuit as shown above.
2. Open `code.ino` in Arduino IDE.
3. Select your board: **Tools → Board → Arduino Uno** (or Mega).
4. Select port: **Tools → Port → COMX**.
5. Upload and open **Serial Monitor** at **9600 baud**.

---

## Expected output

```
Distance: 12.4 cm
Distance: 12.5 cm
Distance: 31.8 cm   ← hand moved closer
Distance: 8.2 cm
```
