const int PIR_PIN    = 3;
const int BUZZER_PIN = 8;
const int LED_ARM    = 6;
const int LED_ALERT  = 7;

bool armed = false;
unsigned long lastMotion = 0;
unsigned long lastBuzz   = 0;
int buzzTone = 500;

void setup() {
  Serial.begin(9600);
  pinMode(PIR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_ARM, OUTPUT);
  pinMode(LED_ALERT, OUTPUT);
  Serial.println("PIR Alarm System Ready.");
  Serial.println("Commands: ARM  DISARM  STATUS");
}

void loop() {
  // Handle serial commands
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n'); cmd.trim();
    cmd.toUpperCase();
    if (cmd == "ARM") {
      armed = true;
      digitalWrite(LED_ARM, HIGH);
      Serial.println("System ARMED.");
    } else if (cmd == "DISARM") {
      armed = false;
      digitalWrite(LED_ARM, LOW);
      digitalWrite(LED_ALERT, LOW);
      noTone(BUZZER_PIN);
      buzzTone = 500;
      Serial.println("System DISARMED.");
    } else if (cmd == "STATUS") {
      Serial.print("Status: "); Serial.println(armed ? "ARMED" : "DISARMED");
    }
  }

  if (!armed) return;

  bool motion = digitalRead(PIR_PIN) == HIGH;

  if (motion) {
    unsigned long now = millis();
    // 3 second debounce check
    if (now - lastMotion > 3000) {
      Serial.println("!! MOTION DETECTED !!");
    }
    lastMotion = now;
    digitalWrite(LED_ALERT, HIGH);

    // Escalating tone: increases every 500ms
    if (now - lastBuzz > 500) {
      buzzTone = constrain(buzzTone + 100, 500, 2000);
      tone(BUZZER_PIN, buzzTone, 200);
      lastBuzz = now;
    }
  } else {
    // Motion cleared — reset alert after 5s of quiet
    if (millis() - lastMotion > 5000 && lastMotion != 0) {
      digitalWrite(LED_ALERT, LOW);
      noTone(BUZZER_PIN);
      buzzTone = 500;
    }
  }
}
