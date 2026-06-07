#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

SoftwareSerial fpSerial(2, 3); // RX, TX
Adafruit_Fingerprint finger(&fpSerial);
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int RELAY_PIN   = A0; // Active-LOW
const int LED_GREEN   = 11;
const int LED_RED     = 12;
const int BUZZER_PIN  = 13;
const int CONFIDENCE_THRESHOLD = 50;

void beep(int freq, int dur) { tone(BUZZER_PIN, freq, dur); delay(dur + 50); }

void showLCD(String row0, String row1 = "") {
  lcd.clear();
  lcd.setCursor(0,0); lcd.print(row0);
  if (row1 != "") { lcd.setCursor(0,1); lcd.print(row1); }
}

void grantAccess(uint8_t id, uint16_t conf) {
  showLCD("** UNLOCKED **", "ID:" + String(id) + " C:" + String(conf));
  digitalWrite(LED_GREEN, HIGH); digitalWrite(RELAY_PIN, LOW);
  beep(1200, 80); delay(100); beep(1500, 80);
  Serial.print("GRANTED ID:"); Serial.print(id); Serial.print(" Conf:"); Serial.println(conf);
  delay(3000);
  digitalWrite(LED_GREEN, LOW); digitalWrite(RELAY_PIN, HIGH);
  showLCD("Place finger...");
}

void denyAccess() {
  showLCD("ACCESS DENIED", "Unknown finger");
  digitalWrite(LED_RED, HIGH);
  beep(300, 500);
  Serial.println("DENIED — no match");
  delay(1500);
  digitalWrite(LED_RED, LOW);
  showLCD("Place finger...");
}

uint8_t enrollFinger(uint8_t id) {
  showLCD("Enroll ID:" + String(id), "Touch finger...");
  Serial.print("Enrolling ID "); Serial.println(id);

  while (finger.getImage() != FINGERPRINT_OK) delay(100);
  if (finger.image2Tz(1) != FINGERPRINT_OK) return 0;

  showLCD("Remove finger");
  delay(1500);
  while (finger.getImage() != FINGERPRINT_NOFINGER) delay(100);

  showLCD("Touch again...");
  while (finger.getImage() != FINGERPRINT_OK) delay(100);
  if (finger.image2Tz(2) != FINGERPRINT_OK) return 0;
  if (finger.createModel() != FINGERPRINT_OK) return 0;

  uint8_t result = finger.storeModel(id);
  if (result == FINGERPRINT_OK) {
    showLCD("Enrolled OK!", "ID:" + String(id));
    beep(800, 100); beep(1000, 100); beep(1200, 100);
    Serial.print("Enrolled ID:"); Serial.println(id);
    delay(2000);
  }
  return result;
}

void handleSerial() {
  if (!Serial.available()) return;
  String cmd = Serial.readStringUntil('\n'); cmd.trim();

  if (cmd.startsWith("ENROLL:")) {
    uint8_t id = cmd.substring(7).toInt();
    if (id < 1 || id > 127) { Serial.println("ID must be 1-127"); return; }
    enrollFinger(id);

  } else if (cmd.startsWith("DELETE:")) {
    uint8_t id = cmd.substring(7).toInt();
    if (finger.deleteModel(id) == FINGERPRINT_OK) {
      Serial.print("Deleted ID:"); Serial.println(id);
    } else Serial.println("Delete failed");

  } else if (cmd == "LIST") {
    Serial.println("Stored fingerprint IDs:");
    for (int i = 1; i <= 127; i++) {
      if (finger.loadModel(i) == FINGERPRINT_OK) Serial.println(i);
    }

  } else if (cmd == "CLEAR") {
    if (finger.emptyDatabase() == FINGERPRINT_OK) Serial.println("Database cleared");
    else Serial.println("Clear failed");

  } else if (cmd == "STATUS") {
    finger.getParameters();
    Serial.print("Templates stored: "); Serial.println(finger.templateCount);
  }

  showLCD("Place finger...");
}

void setup() {
  Serial.begin(9600);
  fpSerial.begin(57600);
  Wire.begin();
  lcd.init(); lcd.backlight();
  pinMode(RELAY_PIN, OUTPUT); digitalWrite(RELAY_PIN, HIGH);
  pinMode(LED_GREEN, OUTPUT); pinMode(LED_RED, OUTPUT);
  finger.begin(57600);

  if (!finger.verifyPassword()) {
    showLCD("FP sensor error!"); while(1);
  }
  finger.getParameters();
  Serial.print("Templates stored: "); Serial.println(finger.templateCount);
  Serial.println("Commands: ENROLL:1  DELETE:1  LIST  CLEAR  STATUS");
  showLCD("Place finger...");
}

void loop() {
  handleSerial();

  if (finger.getImage() != FINGERPRINT_OK) return;
  if (finger.image2Tz() != FINGERPRINT_OK) return;
  if (finger.fingerFastSearch() == FINGERPRINT_OK) {
    if (finger.confidence >= CONFIDENCE_THRESHOLD) grantAccess(finger.fingerID, finger.confidence);
    else denyAccess();
  } else denyAccess();
}
