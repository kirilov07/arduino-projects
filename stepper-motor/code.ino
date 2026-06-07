#include <Stepper.h>

const int STEPS_PER_REV = 2048;
Stepper stepper(STEPS_PER_REV, 8, 10, 9, 11); // In1,In3,In2,In4 order for 28BYJ-48

void setup() {
  Serial.begin(9600);
  stepper.setSpeed(10); // RPM (max ~15 for 28BYJ-48)
  Serial.println("Stepper Motor Ready.");
  Serial.println("Commands:");
  Serial.println("  ROT:90        - Rotate 90 degrees CW");
  Serial.println("  ROT:-180      - Rotate 180 degrees CCW");
  Serial.println("  REV:2         - Rotate N full revolutions CW");
  Serial.println("  SPIN:CW       - Spin continuously CW (Ctrl+Z to send STOP)");
  Serial.println("  SPIN:CCW      - Spin continuously CCW");
  Serial.println("  SPEED:5       - Set speed in RPM (1-15)");
}

bool spinning = false;
int spinDir = 1;

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n'); cmd.trim();

    if (cmd.startsWith("ROT:")) {
      float degrees = cmd.substring(4).toFloat();
      long steps = (long)((degrees / 360.0) * STEPS_PER_REV);
      Serial.print("Rotating "); Serial.print(degrees); Serial.print("° ("); Serial.print(steps); Serial.println(" steps)");
      stepper.step(steps);
      Serial.println("Done.");
      spinning = false;

    } else if (cmd.startsWith("REV:")) {
      int revs = cmd.substring(4).toInt();
      Serial.print("Rotating "); Serial.print(revs); Serial.println(" revolution(s)");
      stepper.step(revs * STEPS_PER_REV);
      Serial.println("Done.");
      spinning = false;

    } else if (cmd == "SPIN:CW") {
      spinning = true; spinDir = 1;
      Serial.println("Spinning CW — send STOP to halt");

    } else if (cmd == "SPIN:CCW") {
      spinning = true; spinDir = -1;
      Serial.println("Spinning CCW — send STOP to halt");

    } else if (cmd == "STOP") {
      spinning = false;
      Serial.println("Stopped.");

    } else if (cmd.startsWith("SPEED:")) {
      int spd = constrain(cmd.substring(6).toInt(), 1, 15);
      stepper.setSpeed(spd);
      Serial.print("Speed: "); Serial.print(spd); Serial.println(" RPM");
    }
  }

  if (spinning) {
    stepper.step(spinDir * 8); // Small steps for responsive stop
  }
}
