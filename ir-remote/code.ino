#include <IRremote.h>

const int IR_PIN = 11;
const int LED1   = 3;
const int LED2   = 4;
const int LED3   = 5;
const int BUZZ   = 8;

IRrecv irRecv(IR_PIN);
decode_results results;

// Map your remote button codes here — run once in LEARN mode to find them
// These are typical NEC codes for a common mini remote
const unsigned long CODE_POWER = 0xFFA25D;
const unsigned long CODE_VOL_UP = 0xFF629D;
const unsigned long CODE_VOL_DN = 0xFFA857;
const unsigned long CODE_1 = 0xFF6897;
const unsigned long CODE_2 = 0xFF9867;
const unsigned long CODE_3 = 0xFFB04F;
const unsigned long CODE_OK = 0xFF02FD;

bool learnMode = false;
bool powerOn = true;
int brightness = 128;

void handleCode(unsigned long code) {
  if (code == 0xFFFFFFFF) return; // Repeat signal

  if (learnMode) {
    Serial.print("Button code: 0x"); Serial.println(code, HEX);
    return;
  }

  if (code == CODE_POWER) {
    powerOn = !powerOn;
    for (int i=3;i<=5;i++) digitalWrite(i, LOW);
    Serial.println(powerOn ? "Power ON" : "Power OFF");
    tone(BUZZ, 800, 80);

  } else if (!powerOn) return;

  else if (code == CODE_VOL_UP) {
    brightness = min(255, brightness + 25);
    analogWrite(LED1, brightness); analogWrite(LED2, brightness); analogWrite(LED3, brightness);
    Serial.print("Brightness: "); Serial.println(brightness);

  } else if (code == CODE_VOL_DN) {
    brightness = max(0, brightness - 25);
    analogWrite(LED1, brightness); analogWrite(LED2, brightness); analogWrite(LED3, brightness);
    Serial.print("Brightness: "); Serial.println(brightness);

  } else if (code == CODE_1) {
    digitalWrite(LED1, !digitalRead(LED1));
    Serial.print("LED1: "); Serial.println(digitalRead(LED1) ? "ON" : "OFF");

  } else if (code == CODE_2) {
    digitalWrite(LED2, !digitalRead(LED2));
    Serial.print("LED2: "); Serial.println(digitalRead(LED2) ? "ON" : "OFF");

  } else if (code == CODE_3) {
    digitalWrite(LED3, !digitalRead(LED3));
    Serial.print("LED3: "); Serial.println(digitalRead(LED3) ? "ON" : "OFF");

  } else if (code == CODE_OK) {
    tone(BUZZ, 1000, 100);
    Serial.println("OK pressed");

  } else {
    Serial.print("Unknown code: 0x"); Serial.println(code, HEX);
  }
}

void setup() {
  Serial.begin(9600);
  irRecv.enableIRIn();
  for (int i=3;i<=5;i++) { pinMode(i, OUTPUT); analogWrite(i, brightness); }
  pinMode(BUZZ, OUTPUT);
  Serial.println("IR Remote Control Ready.");
  Serial.println("Type LEARN to decode your remote's button codes.");
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n'); cmd.trim();
    if (cmd == "LEARN") {
      learnMode = !learnMode;
      Serial.println(learnMode ? "Learn mode ON — press remote buttons" : "Learn mode OFF");
    }
  }

  if (irRecv.decode(&results)) {
    handleCode(results.value);
    irRecv.resume();
  }
}
