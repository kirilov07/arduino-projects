const int DATA_PIN  = 11;
const int CLOCK_PIN = 12;
const int LATCH_PIN = 8;

void shiftOut595(byte val) {
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, val);
  digitalWrite(LATCH_PIN, HIGH);
}

String mode = "knight";

void chaser() {
  for (int i = 0; i < 8; i++) { shiftOut595(1 << i); delay(80); }
  for (int i = 6; i > 0; i--) { shiftOut595(1 << i); delay(80); }
}

void binary() {
  for (int i = 0; i < 256; i++) { shiftOut595(i); delay(60); }
}

void knight() {
  byte k[] = {
    0b00000001, 0b00000011, 0b00000111, 0b00001110,
    0b00011100, 0b00111000, 0b01110000, 0b11100000,
    0b01110000, 0b00111000, 0b00011100, 0b00001110,
    0b00000111, 0b00000011
  };
  for (int i = 0; i < 14; i++) { shiftOut595(k[i]); delay(70); }
}

void setup() {
  Serial.begin(9600);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  shiftOut595(0);
  Serial.println("74HC595 Shift Register Ready.");
  Serial.println("Modes: chaser  binary  knight  byte:<0-255>");
}

void loop() {
  if (Serial.available()) {
    mode = Serial.readStringUntil('\n');
    mode.trim();
    if (mode.startsWith("byte:")) {
      byte val = (byte)mode.substring(5).toInt();
      shiftOut595(val);
      Serial.print("Pattern: 0b"); Serial.println(val, BIN);
      mode = ""; // Pause after manual byte
      return;
    }
    Serial.print("Mode: "); Serial.println(mode);
  }

  if (mode == "chaser") chaser();
  else if (mode == "binary") binary();
  else if (mode == "knight") knight();
  else if (mode != "") delay(50);
}
