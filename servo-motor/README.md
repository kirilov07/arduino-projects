# Servo Motor — SG90 / MG996R

Precise angular control of a servo motor using PWM signals. Demonstrates sweep, target position, and serial-controlled positioning — the same control pattern used in robotics and hardware automation.

---

## Demo

<!-- Add your photo or video here -->
> 📷 _Drop a photo or video into `assets/` and link it here_

---

## Components

| Component | Quantity |
|-----------|----------|
| Arduino Uno / Mega | 1 |
| Servo Motor (SG90 or MG996R) | 1 |
| External 5V power supply (for MG996R) | optional |
| Jumper wires | 3 |

---

## Wiring

| Servo Wire | Arduino |
|------------|---------|
| Red (VCC) | 5V (or external 5V for MG996R) |
| Brown / Black (GND) | GND |
| Orange / Yellow (Signal) | Pin 9 |

> ⚠️ The MG996R draws up to 1A under load. Use an external 5V supply — powering it from Arduino's 5V pin can reset the board.

---

## How it works

Servos are controlled by PWM pulses:
- **1ms pulse** → 0°
- **1.5ms pulse** → 90° (center)
- **2ms pulse** → 180°

The Arduino `Servo` library handles the PWM timing. The library maps `write(angle)` (0–180) to the correct pulse width automatically.

---

## Code

```cpp
#include <Servo.h>

Servo myServo;
const int SERVO_PIN = 9;

void setup() {
  Serial.begin(9600);
  myServo.attach(SERVO_PIN);
  Serial.println("Send angle (0-180) via Serial:");
}

void loop() {
  // Sweep demo on startup
  for (int angle = 0; angle <= 180; angle += 5) {
    myServo.write(angle);
    delay(30);
  }
  for (int angle = 180; angle >= 0; angle -= 5) {
    myServo.write(angle);
    delay(30);
  }

  // Serial control
  if (Serial.available()) {
    int angle = Serial.parseInt();
    if (angle >= 0 && angle <= 180) {
      myServo.write(angle);
      Serial.print("Moved to: "); Serial.print(angle); Serial.println("°");
    }
  }
}
```

---

## How to run

1. Wire servo as shown above.
2. Upload `code.ino`.
3. The servo will sweep 0°→180°→0° on loop.
4. Open Serial Monitor at **9600 baud** and type an angle (0–180) + Enter to jump to that position.
