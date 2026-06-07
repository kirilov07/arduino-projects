#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN  10
#define RST_PIN 9
#define GREEN_LED 4
#define RED_LED   5
#define BUZZER    6

MFRC522 rfid(SS_PIN, RST_PIN);

// Authorized card UIDs — add yours here
String authorizedUIDs[] = {
  "AB CD EF 01",
  "12 34 56 78"
};
const int AUTH_COUNT = 2;

String getUID() {
  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) uid += "0";
    uid += String(rfid.uid.uidByte[i], HEX);
    if (i < rfid.uid.size - 1) uid += " ";
  }
  uid.toUpperCase();
  return uid;
}

bool isAuthorized(String uid) {
  for (int i = 0; i < AUTH_COUNT; i++) {
    if (uid == authorizedUIDs[i]) return true;
  }
  return false;
}

void grantAccess() {
  Serial.println("  >> ACCESS GRANTED");
  digitalWrite(GREEN_LED, HIGH);
  tone(BUZZER, 1000, 150);
  delay(1000);
  digitalWrite(GREEN_LED, LOW);
}

void denyAccess() {
  Serial.println("  >> ACCESS DENIED");
  digitalWrite(RED_LED, HIGH);
  tone(BUZZER, 300, 500);
  delay(1000);
  digitalWrite(RED_LED, LOW);
}

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  Serial.println("RFID Access Control Ready. Tap a card...");
  Serial.println("Firmware: " + String(rfid.PCD_ReadRegister(rfid.VersionReg), HEX));
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;

  String uid = getUID();
  Serial.print("Card UID: "); Serial.println(uid);

  if (isAuthorized(uid)) grantAccess();
  else denyAccess();

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
