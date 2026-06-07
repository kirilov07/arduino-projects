// Smart Parking Sensor — HC-SR04 + 3 LEDs + Buzzer
// Wiring: TRIG→9, ECHO→10, Green→4, Yellow→5, Red→6, Buzzer→8

const int TRIG_PIN   = 9;
const int ECHO_PIN   = 10;
const int LED_GREEN  = 4;
const int LED_YELLOW = 5;
const int LED_RED    = 6;
const int BUZZER     = 8;

const float CLEAR   = 50.0; // cm — safe zone
const float WARNING = 20.0; // cm — slow beep
const float DANGER  = 10.0; // cm — fast beep + alarm

float measureDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long us = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms timeout
  if (us == 0) return -1;                   // Out of range
  return us * 0.0343 / 2.0;
}

void setLEDs(bool g, bool y, bool r) {
  digitalWrite(LED_GREEN,  g);
  digitalWrite(LED_YELLOW, y);
  digitalWrite(LED_RED,    r);
}

void printBar(float dist) {
  int bars = constrain((int)(dist / 5), 0, 20);
  Serial.print("[");
  for (int i = 0; i < 20; i++) Serial.print(i < bars ? "#" : " ");
  Serial.print("] ");
  if (dist < 0) { Serial.println("Out of range"); return; }
  Serial.print(dist, 1);
  Serial.println(" cm");
}

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN,   OUTPUT);
  pinMode(ECHO_PIN,   INPUT);
  pinMode(LED_GREEN,  OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_RED,    OUTPUT);
  pinMode(BUZZER,     OUTPUT);
  Serial.println("Smart Parking Sensor — Ready");
}

void loop() {
  float dist = measureDistance();
  printBar(dist);

  if (dist < 0 || dist > 400) {
    // Out of range — all off
    setLEDs(0, 0, 0);
    noTone(BUZZER);
    delay(200);

  } else if (dist > CLEAR) {
    // Safe — green only, silent
    setLEDs(1, 0, 0);
    noTone(BUZZER);
    delay(200);

  } else if (dist > WARNING) {
    // Caution — yellow, 1 Hz beep
    setLEDs(0, 1, 0);
    tone(BUZZER, 1000, 80);
    delay(1000);

  } else if (dist > DANGER) {
    // Warning — red, 4 Hz beep
    setLEDs(0, 0, 1);
    tone(BUZZER, 1500, 60);
    delay(250);

  } else {
    // Critical — all flash + continuous alarm
    bool flash = (millis() / 80) % 2;
    setLEDs(flash, flash, flash);
    tone(BUZZER, 2500);
    delay(80);
  }
}
