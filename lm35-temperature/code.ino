const int LM35_PIN   = A0;
const int WARN_LED   = 7;
const int BUZZ_PIN   = 8;
const float WARN_C   = 30.0;
const float CRIT_C   = 40.0;

float tempMin = 1000, tempMax = -1000;
float prevTemp = -999;
unsigned long lastRead = 0;

float readTempC() {
  long sum = 0;
  for (int i = 0; i < 16; i++) { sum += analogRead(LM35_PIN); delay(2); }
  float voltage = (sum / 16.0) * (5000.0 / 1023.0); // mV
  return voltage / 10.0; // LM35: 10 mV per °C
}

char trendChar(float current, float prev) {
  if (prev < -900) return '~';
  if (current > prev + 0.3) return '^';
  if (current < prev - 0.3) return 'v';
  return '~';
}

void setup() {
  Serial.begin(9600);
  pinMode(WARN_LED, OUTPUT);
  pinMode(BUZZ_PIN, OUTPUT);
  Serial.println("LM35 Thermometer Ready.");
  Serial.println("Temp(C)  Trend  [Bar                ] Min   Max   Status");
}

void loop() {
  if (millis() - lastRead < 500) return;
  lastRead = millis();

  float temp = readTempC();
  tempMin = min(tempMin, temp);
  tempMax = max(tempMax, temp);
  char trend = trendChar(temp, prevTemp);
  prevTemp = temp;

  int bars = map(constrain((int)temp, 0, 50), 0, 50, 0, 20);
  Serial.print(temp, 1); Serial.print("°C  ");
  Serial.print(trend); Serial.print("  [");
  for (int i = 0; i < 20; i++) Serial.print(i < bars ? '#' : ' ');
  Serial.print("] ");
  Serial.print(tempMin, 1); Serial.print("  ");
  Serial.print(tempMax, 1); Serial.print("  ");

  if (temp >= CRIT_C) {
    Serial.println("CRITICAL");
    digitalWrite(WARN_LED, HIGH);
    tone(BUZZ_PIN, 1000, 200);
  } else if (temp >= WARN_C) {
    Serial.println("WARN");
    digitalWrite(WARN_LED, HIGH);
    noTone(BUZZ_PIN);
  } else {
    Serial.println("OK");
    digitalWrite(WARN_LED, LOW);
    noTone(BUZZ_PIN);
  }
}
