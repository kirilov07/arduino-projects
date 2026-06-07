// HC-SR04 Ultrasonic Sensor — Basic Distance Meter
// Wiring: VCC→5V, GND→GND, TRIG→Pin9, ECHO→Pin10

const int TRIG_PIN = 9;
const int ECHO_PIN = 10;

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  // Send trigger pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Measure echo duration
  long duration = pulseIn(ECHO_PIN, HIGH);

  // Calculate distance (speed of sound = 0.0343 cm/µs)
  float distance = duration * 0.0343 / 2.0;

  if (distance < 2 || distance > 400) {
    Serial.println("Out of range");
  } else {
    Serial.print("Distance: ");
    Serial.print(distance, 1);
    Serial.println(" cm");
  }

  delay(500);
}
