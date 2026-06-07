// Relay Module Control — Serial-commanded on/off switching
// Wiring: IN1→Pin7, VCC→5V, GND→GND
// Most 5V relay modules are ACTIVE-LOW (LOW = ON, HIGH = OFF)

const int RELAY_PIN = 7;

void setup() {
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Ensure relay starts OFF
  Serial.println("Relay Control Ready");
  Serial.println("Commands: '1' = ON, '0' = OFF, 'b' = blink 3x");
}

void loop() {
  if (Serial.available()) {
    char cmd = Serial.read();

    switch (cmd) {
      case '1':
        digitalWrite(RELAY_PIN, LOW); // Active-LOW: LOW = ON
        Serial.println("Relay ON");
        break;

      case '0':
        digitalWrite(RELAY_PIN, HIGH);
        Serial.println("Relay OFF");
        break;

      case 'b':
        Serial.println("Blinking 3x...");
        for (int i = 0; i < 3; i++) {
          digitalWrite(RELAY_PIN, LOW);
          delay(300);
          digitalWrite(RELAY_PIN, HIGH);
          delay(300);
        }
        Serial.println("Done");
        break;
    }
  }
}
