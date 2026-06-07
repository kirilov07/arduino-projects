// Pan-Tilt Servo Controller — Serial command protocol
// Commands: PAN:<0-180>  TILT:<0-180>  CENTER  SWEEP
// Wiring: PAN servo→Pin9, TILT servo→Pin10

#include <Servo.h>

Servo servoPan;
Servo servoTilt;

const int PAN_PIN  = 9;
const int TILT_PIN = 10;

int currentPan  = 90;
int currentTilt = 90;

void smoothMove(Servo &servo, int &current, int target, int stepDelay = 8) {
  int step = (target > current) ? 1 : -1;
  while (current != target) {
    current += step;
    servo.write(current);
    delay(stepDelay);
  }
}

void parseCommand(String cmd) {
  cmd.trim();
  cmd.toUpperCase();

  if (cmd.startsWith("PAN:")) {
    int angle = constrain(cmd.substring(4).toInt(), 0, 180);
    smoothMove(servoPan, currentPan, angle);
    Serial.print("PAN -> "); Serial.println(angle);

  } else if (cmd.startsWith("TILT:")) {
    int angle = constrain(cmd.substring(5).toInt(), 0, 180);
    smoothMove(servoTilt, currentTilt, angle);
    Serial.print("TILT -> "); Serial.println(angle);

  } else if (cmd == "CENTER") {
    smoothMove(servoPan,  currentPan,  90);
    smoothMove(servoTilt, currentTilt, 90);
    Serial.println("Centered (90, 90)");

  } else if (cmd == "SWEEP") {
    Serial.println("Sweeping...");
    for (int a = 0; a <= 180; a += 2) {
      servoPan.write(a); servoTilt.write(a); delay(15);
    }
    for (int a = 180; a >= 0; a -= 2) {
      servoPan.write(a); servoTilt.write(a); delay(15);
    }
    currentPan = currentTilt = 0;
    Serial.println("Sweep done");

  } else {
    Serial.print("Unknown: "); Serial.println(cmd);
    Serial.println("Commands: PAN:<0-180>  TILT:<0-180>  CENTER  SWEEP");
  }
}

void setup() {
  Serial.begin(9600);
  servoPan.attach(PAN_PIN);
  servoTilt.attach(TILT_PIN);
  servoPan.write(90);
  servoTilt.write(90);
  delay(500);
  Serial.println("Pan-Tilt Controller Ready");
  Serial.println("Commands: PAN:<0-180>  TILT:<0-180>  CENTER  SWEEP");
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    parseCommand(cmd);
  }
}
