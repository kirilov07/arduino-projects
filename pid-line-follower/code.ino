// 5-sensor IR array PID line follower
// Sensors: A0(leftmost=-2) A1(-1) A2(0) A3(+1) A4(rightmost=+2)
// Motor driver L298N

const int ENA = 3, IN1 = 8,  IN2 = 9;   // Left motor
const int ENB = 5, IN3 = 11, IN4 = 12;  // Right motor
const int SENSOR_PINS[5] = {A0, A1, A2, A3, A4};
const int WEIGHTS[5] = {-2, -1, 0, 1, 2};

int BASE_SPEED = 120;
float Kp = 25.0, Ki = 0.0, Kd = 15.0;

float integral = 0, prevError = 0;
bool running = false;
unsigned long lastLoop = 0;

void motorLeft(int speed) {
  speed = constrain(speed, -255, 255);
  analogWrite(ENA, abs(speed));
  digitalWrite(IN1, speed >= 0 ? HIGH : LOW);
  digitalWrite(IN2, speed >= 0 ? LOW  : HIGH);
}

void motorRight(int speed) {
  speed = constrain(speed, -255, 255);
  analogWrite(ENB, abs(speed));
  digitalWrite(IN3, speed >= 0 ? HIGH : LOW);
  digitalWrite(IN4, speed >= 0 ? LOW  : HIGH);
}

void stop() { motorLeft(0); motorRight(0); }

float readPosition() {
  int sum = 0, weightedSum = 0, count = 0;
  for (int i = 0; i < 5; i++) {
    int val = (digitalRead(SENSOR_PINS[i]) == LOW) ? 1 : 0; // LOW = on line
    weightedSum += val * WEIGHTS[i];
    sum += val;
    count += val;
  }
  if (count == 0) return prevError; // Lost line — keep last error
  if (count == 5) return 0;         // All sensors on line — intersection
  return (float)weightedSum / sum;
}

void handleSerial() {
  if (!Serial.available()) return;
  String cmd = Serial.readStringUntil('\n'); cmd.trim();
  if      (cmd == "GO")         { running = true;  integral = 0; Serial.println("Running"); }
  else if (cmd == "STOP")       { running = false; stop(); Serial.println("Stopped"); }
  else if (cmd.startsWith("KP:"))    { Kp = cmd.substring(3).toFloat(); Serial.print("Kp:"); Serial.println(Kp); }
  else if (cmd.startsWith("KD:"))    { Kd = cmd.substring(3).toFloat(); Serial.print("Kd:"); Serial.println(Kd); }
  else if (cmd.startsWith("KI:"))    { Ki = cmd.substring(3).toFloat(); Serial.print("Ki:"); Serial.println(Ki); }
  else if (cmd.startsWith("SPEED:")) { BASE_SPEED = cmd.substring(6).toInt(); Serial.print("Speed:"); Serial.println(BASE_SPEED); }
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 5; i++) pinMode(SENSOR_PINS[i], INPUT);
  for (int p : {ENA, ENB}) pinMode(p, OUTPUT);
  for (int p : {IN1, IN2, IN3, IN4}) pinMode(p, OUTPUT);
  stop();
  Serial.println("PID Line Follower Ready. Type GO to start, STOP to halt.");
  Serial.println("Tune: KP:25  KD:15  KI:0  SPEED:120");
}

void loop() {
  handleSerial();
  if (!running) return;
  if (millis() - lastLoop < 10) return;
  lastLoop = millis();

  float error = readPosition();
  integral += error * 0.01;
  integral = constrain(integral, -50, 50);
  float derivative = (error - prevError) / 0.01;
  prevError = error;

  float correction = Kp * error + Ki * integral + Kd * derivative;

  motorLeft(BASE_SPEED  - (int)correction);
  motorRight(BASE_SPEED + (int)correction);
}
