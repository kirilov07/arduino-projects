#include <Servo.h>

const int JOY_X = A0, JOY_Y = A1, JOY_BTN = 2;
Servo panServo, tiltServo;
int panAngle = 90, tiltAngle = 90;

int applyDeadzone(int val, int center = 512, int dz = 40) {
  if (abs(val - center) < dz) return center;
  return val;
}

void setup() {
  Serial.begin(9600);
  panServo.attach(9);
  tiltServo.attach(10);
  panServo.write(90);
  tiltServo.write(90);
  pinMode(JOY_BTN, INPUT_PULLUP);
  Serial.println("Joystick Pan-Tilt Controller Ready.");
  Serial.println("Move joystick to control servos. Click to center.");
}

void loop() {
  int x = applyDeadzone(analogRead(JOY_X));
  int y = applyDeadzone(analogRead(JOY_Y));
  bool btn = digitalRead(JOY_BTN) == LOW;

  if (btn) {
    panAngle = tiltAngle = 90;
    panServo.write(90);
    tiltServo.write(90);
    Serial.println("Centered.");
    delay(500);
    return;
  }

  int targetPan  = map(x, 0, 1023, 0, 180);
  int targetTilt = map(y, 0, 1023, 0, 180);

  // Smooth interpolation — avoids abrupt jumps
  panAngle  += constrain(targetPan  - panAngle,  -3, 3);
  tiltAngle += constrain(targetTilt - tiltAngle, -3, 3);

  panServo.write(panAngle);
  tiltServo.write(tiltAngle);

  Serial.print("Pan:"); Serial.print(panAngle);
  Serial.print("  Tilt:"); Serial.println(tiltAngle);

  delay(20);
}
