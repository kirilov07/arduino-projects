#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// MAX6675 — software SPI
const int MAX_CS  = 5;
const int MAX_SCK = 6;
const int MAX_SO  = 4;
const int SSR_PIN = 9; // PWM output to SSR

LiquidCrystal_I2C lcd(0x27, 16, 2);

// PID parameters (tune via serial)
float Kp = 2.0, Ki = 0.05, Kd = 1.0;
float setpoint = 100.0;
bool autoMode = true;
int manualOutput = 0;

float integral = 0, prevError = 0;
unsigned long lastPID = 0;
const unsigned long PID_INTERVAL = 100; // ms

// Output window for SSR duty cycling
const unsigned long WINDOW_SIZE = 250; // ms
unsigned long windowStart = 0;
int output = 0; // 0–255

float readMAX6675() {
  digitalWrite(MAX_CS, LOW);
  delayMicroseconds(2);
  uint16_t raw = 0;
  for (int i = 15; i >= 0; i--) {
    digitalWrite(MAX_SCK, LOW); delayMicroseconds(1);
    if (digitalRead(MAX_SO)) raw |= (1 << i);
    digitalWrite(MAX_SCK, HIGH); delayMicroseconds(1);
  }
  digitalWrite(MAX_CS, HIGH);
  if (raw & 0x4) return -1; // Fault bit
  raw >>= 3;
  return raw * 0.25; // 0.25°C resolution
}

void computePID(float current) {
  float error = setpoint - current;
  integral += error * (PID_INTERVAL / 1000.0);
  integral = constrain(integral, -255.0 / Ki, 255.0 / Ki); // Windup clamp
  float derivative = (error - prevError) / (PID_INTERVAL / 1000.0);
  prevError = error;
  float raw = Kp * error + Ki * integral + Kd * derivative;
  output = constrain((int)raw, 0, 255);
}

void driveSSR() {
  unsigned long now = millis();
  if (now - windowStart >= WINDOW_SIZE) windowStart = now;
  // On-time proportional to output within window
  int onTime = (int)((float)output / 255.0 * WINDOW_SIZE);
  digitalWrite(SSR_PIN, (now - windowStart < (unsigned long)onTime) ? HIGH : LOW);
}

void handleSerial() {
  if (!Serial.available()) return;
  String cmd = Serial.readStringUntil('\n'); cmd.trim();
  if      (cmd.startsWith("SP:"))     { setpoint = cmd.substring(3).toFloat(); Serial.print("SP: "); Serial.println(setpoint); }
  else if (cmd.startsWith("KP:"))     { Kp = cmd.substring(3).toFloat(); Serial.print("Kp: "); Serial.println(Kp); }
  else if (cmd.startsWith("KI:"))     { Ki = cmd.substring(3).toFloat(); Serial.print("Ki: "); Serial.println(Ki); }
  else if (cmd.startsWith("KD:"))     { Kd = cmd.substring(3).toFloat(); Serial.print("Kd: "); Serial.println(Kd); }
  else if (cmd == "AUTO")             { autoMode = true; integral = 0; Serial.println("Auto mode"); }
  else if (cmd.startsWith("MANUAL:")) { autoMode = false; manualOutput = constrain(cmd.substring(7).toInt(), 0, 100); output = map(manualOutput,0,100,0,255); Serial.print("Manual: "); Serial.println(manualOutput); }
}

void setup() {
  Serial.begin(9600);
  lcd.init(); lcd.backlight();
  pinMode(MAX_CS, OUTPUT); pinMode(MAX_SCK, OUTPUT); pinMode(MAX_SO, INPUT);
  pinMode(SSR_PIN, OUTPUT);
  digitalWrite(MAX_CS, HIGH);
  windowStart = millis();
  Serial.println("PID Temperature Controller Ready.");
  Serial.println("Commands: SP:150  KP:2.5  KI:0.05  KD:1.0  AUTO  MANUAL:50");
}

void loop() {
  handleSerial();
  driveSSR();

  if (millis() - lastPID < PID_INTERVAL) return;
  lastPID = millis();

  float temp = readMAX6675();
  if (temp < 0) { Serial.println("Thermocouple fault!"); return; }

  if (autoMode) computePID(temp);

  float error = setpoint - temp;
  int pct = map(output, 0, 255, 0, 100);

  lcd.setCursor(0, 0);
  lcd.print("SP:"); lcd.print((int)setpoint); lcd.print(" PV:"); lcd.print(temp, 1); lcd.print("   ");
  lcd.setCursor(0, 1);
  lcd.print("Err:"); lcd.print(error, 1); lcd.print(" Out:"); lcd.print(pct); lcd.print("%  ");

  Serial.print("T:"); Serial.print(temp,1);
  Serial.print(" SP:"); Serial.print(setpoint,1);
  Serial.print(" E:"); Serial.print(error,1);
  Serial.print(" Out:"); Serial.print(pct);
  Serial.println("%");
}
