// IR Obstacle Sensor — Proximity Detection
// Wiring: OUT→Pin8, VCC→5V, GND→GND
// Output is ACTIVE-LOW: LOW = obstacle detected

const int IR_PIN   = 8;
const int LED_PIN  = 13; // Built-in LED as indicator

void setup() {
  Serial.begin(9600);
  pinMode(IR_PIN,  INPUT);
  pinMode(LED_PIN, OUTPUT);
  Serial.println("IR Obstacle Sensor Ready");
  Serial.println("Adjust potentiometer on module to set detection range.");
  Serial.println("---");
}

void loop() {
  int state = digitalRead(IR_PIN);

  if (state == LOW) { // Active-LOW: obstacle in range
    digitalWrite(LED_PIN, HIGH);
    Serial.println("OBSTACLE DETECTED");
  } else {
    digitalWrite(LED_PIN, LOW);
    Serial.println("Clear");
  }

  delay(200);
}
