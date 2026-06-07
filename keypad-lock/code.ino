#include <Keypad.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const byte ROWS = 4, COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int RELAY_PIN    = A0;  // Active-LOW
const int LED_GREEN    = 11;
const int LED_RED      = 12;
const int BUZZER_PIN   = 13;
const String PASSWORD  = "1234";

String input = "";
int failCount = 0;
bool locked = false;
unsigned long lockUntil = 0;
const int MAX_FAILS     = 3;
const unsigned long LOCK_DURATION = 30000UL;

void accessGranted() {
  lcd.clear(); lcd.print("** UNLOCKED **");
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(RELAY_PIN, LOW);
  tone(BUZZER_PIN, 1200, 100); delay(150); tone(BUZZER_PIN, 1500, 100);
  delay(3000);
  digitalWrite(LED_GREEN, LOW);
  digitalWrite(RELAY_PIN, HIGH);
  failCount = 0;
  lcd.clear(); lcd.print("Enter PIN:");
}

void accessDenied() {
  failCount++;
  lcd.clear(); lcd.print("WRONG PIN");
  lcd.setCursor(0,1); lcd.print("Attempts: "); lcd.print(failCount);
  digitalWrite(LED_RED, HIGH);
  tone(BUZZER_PIN, 300, 600);
  delay(1500);
  digitalWrite(LED_RED, LOW);
  if (failCount >= MAX_FAILS) {
    locked = true;
    lockUntil = millis() + LOCK_DURATION;
    lcd.clear(); lcd.print("LOCKED 30s");
    Serial.println("Lockout activated.");
  } else {
    lcd.clear(); lcd.print("Enter PIN:");
  }
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  lcd.init(); lcd.backlight();
  pinMode(RELAY_PIN, OUTPUT); digitalWrite(RELAY_PIN, HIGH);
  pinMode(LED_GREEN, OUTPUT); pinMode(LED_RED, OUTPUT);
  lcd.print("Enter PIN:"); Serial.println("Keypad Lock Ready");
}

void loop() {
  if (locked) {
    unsigned long remaining = (lockUntil - millis()) / 1000;
    if (millis() >= lockUntil) {
      locked = false; failCount = 0;
      lcd.clear(); lcd.print("Enter PIN:");
    } else {
      lcd.setCursor(0,1); lcd.print("Wait: "); lcd.print(remaining); lcd.print("s  ");
    }
    delay(200); return;
  }

  char key = keypad.getKey();
  if (!key) return;

  if (key == '#') {
    if (input == PASSWORD) accessGranted();
    else accessDenied();
    input = "";
  } else if (key == '*') {
    input = "";
    lcd.setCursor(0,1); lcd.print("                ");
  } else if (input.length() < 8) {
    input += key;
    lcd.setCursor(0,1);
    for (int i=0;i<(int)input.length();i++) lcd.print('*');
  }
}
