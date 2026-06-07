const int MQ2_PIN    = A0;
const int WARN_LED   = 6;
const int DANGER_LED = 7;
const int BUZZER_PIN = 8;
const int FAN_RELAY  = 9; // Active-LOW relay

const int WARMUP_SECS  = 60;
const int WARN_PPM     = 200;
const int DANGER_PPM   = 400;

bool warmedUp = false;
unsigned long startTime;

// Simple voltage-to-PPM approximation for LPG/smoke
// Calibrate Rs/R0 ratio for your sensor in clean air
float sensorResistance(int adcVal) {
  float voltage = adcVal * (5.0 / 1023.0);
  if (voltage < 0.01) voltage = 0.01;
  return (5.0 - voltage) / voltage * 10.0; // Load resistor 10kΩ
}

int toPPM(float rs) {
  // Approximate LPG curve from MQ-2 datasheet
  // ppm = 1000 * (Rs/2.3)^(-2.3)  rough curve fit
  return (int)(1000.0 * pow(rs / 2.3, -2.3));
}

void alert(bool warn, bool danger) {
  digitalWrite(WARN_LED,   warn   ? HIGH : LOW);
  digitalWrite(DANGER_LED, danger ? HIGH : LOW);
  digitalWrite(FAN_RELAY,  danger ? LOW  : HIGH); // Fan on when danger
  if (danger) tone(BUZZER_PIN, 900, 300);
  else if (warn) tone(BUZZER_PIN, 400, 100);
  else noTone(BUZZER_PIN);
}

void setup() {
  Serial.begin(9600);
  pinMode(WARN_LED,   OUTPUT);
  pinMode(DANGER_LED, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(FAN_RELAY,  OUTPUT); digitalWrite(FAN_RELAY, HIGH);
  startTime = millis();
  Serial.println("MQ-2 Gas Sensor — warming up...");
  Serial.print("Wait "); Serial.print(WARMUP_SECS); Serial.println("s before readings are valid.");
}

void loop() {
  unsigned long elapsed = (millis() - startTime) / 1000;

  if (!warmedUp) {
    if (elapsed < WARMUP_SECS) {
      Serial.print("Warmup: "); Serial.print(elapsed); Serial.print("/"); Serial.print(WARMUP_SECS); Serial.println("s");
      delay(2000); return;
    }
    warmedUp = true;
    Serial.println("Sensor ready.");
  }

  int raw = analogRead(MQ2_PIN);
  float rs = sensorResistance(raw);
  int ppm  = toPPM(rs);

  int bars = map(constrain(ppm, 0, 500), 0, 500, 0, 20);
  Serial.print("PPM: "); Serial.print(ppm);
  Serial.print("  [");
  for (int i=0;i<20;i++) Serial.print(i < bars ? (i > 14 ? '!' : '#') : ' ');
  Serial.print("]  ");

  if (ppm >= DANGER_PPM) {
    Serial.println("!! DANGER !!");
    alert(false, true);
  } else if (ppm >= WARN_PPM) {
    Serial.println("WARNING");
    alert(true, false);
  } else {
    Serial.println("OK");
    alert(false, false);
  }

  delay(1000);
}
