# Relay Control Module

Switching high-power devices (lights, fans, pumps, diffusers) on and off from an Arduino digital pin. Relays are the bridge between low-voltage microcontroller logic and mains-powered or high-current devices — the same principle used in S.Y.N.A.P.S.E to control the ultrasonic diffusers.

---

## Demo

<!-- Add your photo or video here -->
> 📷 _Drop a photo or video into `assets/` and link it here_

---

## Components

| Component | Quantity |
|-----------|----------|
| Arduino Uno / Mega | 1 |
| 5V Relay Module (1, 4, or 8 channel) | 1 |
| Device to switch (LED, bulb, fan, pump…) | 1 |
| Jumper wires | 3 |

---

## Wiring

| Relay Module Pin | Arduino |
|-----------------|---------|
| VCC | 5V |
| GND | GND |
| IN1 | Pin 7 |

> ⚠️ Do NOT touch the high-voltage (AC) side of the relay unless you are qualified to work with mains electricity. For demonstration, use a 5V LED or small DC device on the switched side.

---

## How it works

A relay is an electromagnetic switch. When the Arduino sends HIGH (or LOW depending on module type — most 5V relay modules are **active-LOW**) to the IN pin, the relay coil energizes and closes the circuit on the high-power side.

- **Active-LOW module**: `digitalWrite(IN1, LOW)` → relay ON
- **Active-HIGH module**: `digitalWrite(IN1, HIGH)` → relay ON

Check your module's datasheet or test to determine which yours is.

---

## Code

```cpp
const int RELAY_PIN = 7;

void setup() {
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Start OFF (active-LOW module)
  Serial.println("Relay control ready. Send '1' to ON, '0' to OFF.");
}

void loop() {
  if (Serial.available()) {
    char cmd = Serial.read();
    if (cmd == '1') {
      digitalWrite(RELAY_PIN, LOW);  // ON (active-LOW)
      Serial.println("Relay ON");
    } else if (cmd == '0') {
      digitalWrite(RELAY_PIN, HIGH); // OFF
      Serial.println("Relay OFF");
    }
  }
}
```

---

## How to run

1. Wire relay module as shown.
2. Upload `code.ino`, open Serial Monitor at **9600 baud**.
3. Send `1` to activate the relay, `0` to deactivate.
4. You should hear the relay click and see/feel the switched device respond.

---

## Real-world use

This exact pattern (pyserial → Arduino → relay) is the hardware control layer in **S.Y.N.A.P.S.E**, where the Jetson sends commands over USB serial to an Arduino Mega, which switches 8 ultrasonic diffusers via a relay board.
