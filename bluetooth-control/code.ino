#include <SoftwareSerial.h>

SoftwareSerial bt(10, 11); // RX=10, TX=11 (TX→HC-05 RXD via voltage divider)

const int LED1 = 3;
const int LED2 = 4;
const int LED3 = 5;
const int LED4 = 6;

const int leds[] = {LED1, LED2, LED3, LED4};

void sendStatus() {
  bt.print("LED: ");
  for (int i=0;i<4;i++) {
    bt.print(i+1); bt.print("=");
    bt.print(digitalRead(leds[i]) ? "ON" : "OFF");
    if (i < 3) bt.print(" ");
  }
  bt.println();
  Serial.print("Status sent. ");
}

void setup() {
  Serial.begin(9600);
  bt.begin(9600);
  for (int i=0;i<4;i++) { pinMode(leds[i], OUTPUT); digitalWrite(leds[i], LOW); }
  Serial.println("Bluetooth Control Ready. Pair with HC-05, default PIN: 1234");
  Serial.println("Commands: 1-4=toggle LED, 0=all off, A=all on, S=status");
  bt.println("Connected. 1-4=toggle, 0=all off, A=all on, S=status");
}

void loop() {
  if (bt.available()) {
    char cmd = (char)bt.read();
    Serial.print("BT cmd: "); Serial.println(cmd);

    if (cmd >= '1' && cmd <= '4') {
      int idx = cmd - '1';
      bool state = !digitalRead(leds[idx]);
      digitalWrite(leds[idx], state);
      bt.print("LED"); bt.print(cmd); bt.println(state ? " ON" : " OFF");
    } else if (cmd == '0') {
      for (int i=0;i<4;i++) digitalWrite(leds[i], LOW);
      bt.println("All OFF");
    } else if (cmd == 'A' || cmd == 'a') {
      for (int i=0;i<4;i++) digitalWrite(leds[i], HIGH);
      bt.println("All ON");
    } else if (cmd == 'S' || cmd == 's') {
      sendStatus();
    }
  }

  // Also handle Serial for debugging
  if (Serial.available()) {
    bt.write(Serial.read());
  }
}
