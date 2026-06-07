// Servo Motor Control — Sweep + Serial angle input
// Wiring: Signal→Pin9, VCC→5V, GND→GND

#include <Servo.h>

Servo myServo;
const int SERVO_PIN = 9;

void setup() {
  Serial.begin(9600);
  myServo.attach(SERVO_PIN);
  myServo.write(90); // Start at center
  delay(500);
  Serial.println("Servo ready. Sweeping...");
  Serial.println("Type an angle (0-180) + Enter to set position.");
}

void loop() {
  // Continuous sweep
  for (int angle = 0; angle <= 180; angle += 2) {
    myServo.write(angle);
    delay(15);
    if (Serial.available()) break; // Interrupt sweep on input
  }
  for (int angle = 180; angle >= 0; angle -= 2) {
    myServo.write(angle);
    delay(15);
    if (Serial.available()) break;
  }

  // Serial angle control
  if (Serial.available()) {
    int angle = Serial.parseInt();
    Serial.read(); // consume newline
    if (angle >= 0 && angle <= 180) {
      myServo.write(angle);
      Serial.print("Set to: ");
      Serial.print(angle);
      Serial.println("°");
      delay(2000); // Hold position before resuming sweep
    } else {
      Serial.println("Invalid angle. Enter 0–180.");
    }
  }
}
