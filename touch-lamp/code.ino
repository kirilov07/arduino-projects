const int TOUCH_PIN = 2;
const int LED_PIN   = 9; // PWM

const int BRIGHTNESS_STEPS[] = {0, 64, 128, 192, 255};
int stepIdx = 4; // Start at full brightness (ON)
bool lampOn = true;

unsigned long pressStart = 0, lastRelease = 0;
bool wasPressed = false, holding = false;
int holdDir = -1;
int currentBrightness = 255;

void applyBrightness(int b) {
  currentBrightness = constrain(b, 0, 255);
  analogWrite(LED_PIN, lampOn ? currentBrightness : 0);
}

void setup() {
  Serial.begin(9600);
  pinMode(TOUCH_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  applyBrightness(255);
  Serial.println("Touch Lamp Ready.");
  Serial.println("Single tap = cycle brightness | Double tap = toggle | Hold = dim/brighten");
}

void loop() {
  bool touched = digitalRead(TOUCH_PIN) == HIGH;
  unsigned long now = millis();

  if (touched && !wasPressed) {
    pressStart = now; wasPressed = true; holding = false;
  }

  if (touched && wasPressed && !holding && (now - pressStart > 800)) {
    holding = true;
    holdDir = (currentBrightness > 127) ? -1 : 1;
    Serial.println(holdDir < 0 ? "Dimming..." : "Brightening...");
  }

  if (holding && touched) {
    currentBrightness = constrain(currentBrightness + holdDir * 3, 10, 255);
    applyBrightness(currentBrightness);
    delay(20); return;
  }

  if (!touched && wasPressed) {
    wasPressed = false;
    if (!holding) {
      unsigned long pressDuration = now - pressStart;
      unsigned long gap = now - lastRelease;
      if (gap < 300 && gap > 50) {
        lampOn = !lampOn;
        applyBrightness(currentBrightness);
        Serial.println(lampOn ? "Lamp ON" : "Lamp OFF");
      } else if (pressDuration < 500) {
        stepIdx = (stepIdx + 1) % 5;
        applyBrightness(BRIGHTNESS_STEPS[stepIdx]);
        Serial.print("Brightness step: "); Serial.print(stepIdx);
        Serial.print(" ("); Serial.print(BRIGHTNESS_STEPS[stepIdx]); Serial.println(")");
      }
      lastRelease = now;
    }
    holding = false;
  }
}
