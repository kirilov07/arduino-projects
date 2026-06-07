const int HALL_PIN = 2;
volatile unsigned long pulseCount = 0;
unsigned long lastCalc = 0;
float rpm = 0, rpmMin = 99999, rpmMax = 0, rpmSum = 0;
unsigned long samples = 0;

void onPulse() { pulseCount++; }

void setup() {
  Serial.begin(115200);
  pinMode(HALL_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(HALL_PIN), onPulse, FALLING);
  Serial.println("Hall Effect Tachometer — Ready");
  Serial.println("Attach neodymium magnet to rotating shaft.");
  Serial.println("RPM\t\t[Bar            ]\tMin\tMax\tAvg");
}

void loop() {
  if (millis() - lastCalc < 1000) return;
  lastCalc = millis();

  noInterrupts();
  unsigned long count = pulseCount;
  pulseCount = 0;
  interrupts();

  rpm = (float)count * 60.0; // 1 magnet per revolution

  if (rpm > 0) {
    if (samples > 0) rpmMin = min(rpmMin, rpm);
    else rpmMin = rpm;
    rpmMax = max(rpmMax, rpm);
    rpmSum += rpm;
    samples++;
  }

  int bars = map(constrain((int)rpm, 0, 3000), 0, 3000, 0, 16);
  Serial.print("RPM: "); Serial.print((int)rpm);
  Serial.print("\t[");
  for (int i=0;i<16;i++) Serial.print(i < bars ? '#' : ' ');
  Serial.print("]  Min:");
  if (samples > 0) { Serial.print((int)rpmMin); } else { Serial.print("--"); }
  Serial.print("  Max:"); Serial.print((int)rpmMax);
  Serial.print("  Avg:");
  if (samples > 0) Serial.print((int)(rpmSum / samples));
  else Serial.print("--");
  Serial.println();
}
