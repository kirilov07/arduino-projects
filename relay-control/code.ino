// 4-Channel Relay Controller — Serial command protocol
// Active-LOW relay module (LOW = ON, HIGH = OFF)
// Wiring: IN1→4, IN2→5, IN3→6, IN4→7, VCC→5V, GND→GND

const int RELAY_PINS[4] = {4, 5, 6, 7};
bool channelState[4]    = {false, false, false, false};

void setChannel(int ch, bool on) {
  channelState[ch] = on;
  digitalWrite(RELAY_PINS[ch], on ? LOW : HIGH); // Active-LOW
}

void printStatus() {
  for (int i = 0; i < 4; i++) {
    Serial.print("CH"); Serial.print(i + 1);
    Serial.print(": "); Serial.println(channelState[i] ? "ON" : "OFF");
  }
}

void parseCommand(String cmd) {
  cmd.trim();
  cmd.toUpperCase();

  if (cmd == "STATUS")   { printStatus(); return; }
  if (cmd == "ALL:ON")   { for (int i=0;i<4;i++) setChannel(i,true);  Serial.println("All ON");  return; }
  if (cmd == "ALL:OFF")  { for (int i=0;i<4;i++) setChannel(i,false); Serial.println("All OFF"); return; }

  if (!cmd.startsWith("CH") || cmd.length() < 5) { Serial.println("Unknown command"); return; }

  int ch = cmd.charAt(2) - '1';
  if (ch < 0 || ch > 3) { Serial.println("Invalid channel (1-4)"); return; }

  String action = cmd.substring(4); // After "CH1:"

  if (action == "ON") {
    setChannel(ch, true);
    Serial.print("CH"); Serial.print(ch+1); Serial.println(": ON");

  } else if (action == "OFF") {
    setChannel(ch, false);
    Serial.print("CH"); Serial.print(ch+1); Serial.println(": OFF");

  } else if (action == "TOGGLE") {
    setChannel(ch, !channelState[ch]);
    Serial.print("CH"); Serial.print(ch+1);
    Serial.println(channelState[ch] ? ": ON" : ": OFF");

  } else if (action.startsWith("PULSE:")) {
    int ms = action.substring(6).toInt();
    if (ms <= 0) { Serial.println("Invalid duration"); return; }
    setChannel(ch, true);
    Serial.print("CH"); Serial.print(ch+1); Serial.print(": PULSE "); Serial.print(ms); Serial.println("ms");
    delay(ms);
    setChannel(ch, false);
    Serial.print("CH"); Serial.print(ch+1); Serial.println(": OFF");

  } else {
    Serial.println("Unknown action. Use ON / OFF / TOGGLE / PULSE:<ms>");
  }
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 4; i++) {
    pinMode(RELAY_PINS[i], OUTPUT);
    digitalWrite(RELAY_PINS[i], HIGH); // All OFF
  }
  Serial.println("4-Channel Relay Controller Ready");
  Serial.println("Commands: CH1:ON  CH2:OFF  CH3:TOGGLE  CH1:PULSE:2000  ALL:ON  STATUS");
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    parseCommand(cmd);
  }
}
