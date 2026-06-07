#include <Servo.h>

// Motor driver L298N
const int ENA = 3, IN1 = 8,  IN2 = 9;
const int ENB = 5, IN3 = 11, IN4 = 12;

// Ultrasonic
const int TRIG = 7, ECHO = 6;

// Servo for sensor pan
Servo scanServo;
const int SERVO_PIN = 10;

const int SAFE_DIST  = 25; // cm
const int BASE_SPEED = 170;
const int TURN_SPEED = 150;

void motorLeft(int spd)  { spd = constrain(spd,-255,255); analogWrite(ENA,abs(spd)); digitalWrite(IN1,spd>=0); digitalWrite(IN2,spd<0); }
void motorRight(int spd) { spd = constrain(spd,-255,255); analogWrite(ENB,abs(spd)); digitalWrite(IN3,spd>=0); digitalWrite(IN4,spd<0); }
void forward()  { motorLeft(BASE_SPEED);  motorRight(BASE_SPEED); }
void reverse()  { motorLeft(-BASE_SPEED); motorRight(-BASE_SPEED); }
void turnLeft(int ms)  { motorLeft(-TURN_SPEED); motorRight( TURN_SPEED); delay(ms); }
void turnRight(int ms) { motorLeft( TURN_SPEED); motorRight(-TURN_SPEED); delay(ms); }
void stopMotors() { motorLeft(0); motorRight(0); }

float ping(int tries = 3) {
  float readings[3];
  for (int i = 0; i < tries; i++) {
    digitalWrite(TRIG, LOW);  delayMicroseconds(2);
    digitalWrite(TRIG, HIGH); delayMicroseconds(10);
    digitalWrite(TRIG, LOW);
    long dur = pulseIn(ECHO, HIGH, 25000);
    readings[i] = (dur == 0) ? 200 : dur * 0.0343 / 2.0;
    delay(10);
  }
  // Median of 3
  if (readings[0] > readings[1]) { float t=readings[0]; readings[0]=readings[1]; readings[1]=t; }
  if (readings[1] > readings[2]) { float t=readings[1]; readings[1]=readings[2]; readings[2]=t; }
  if (readings[0] > readings[1]) { float t=readings[0]; readings[0]=readings[1]; readings[1]=t; }
  return readings[1];
}

float scanAngles[5] = {0, 45, 90, 135, 180}; // pan angles
float distances[5];

void scanSurroundings() {
  for (int i = 0; i < 5; i++) {
    scanServo.write((int)scanAngles[i]);
    delay(200); // Servo settle
    distances[i] = ping();
    Serial.print(scanAngles[i]); Serial.print("°:"); Serial.print(distances[i],0); Serial.print("cm  ");
  }
  Serial.println();
  scanServo.write(90); // Center
}

void decideEscape() {
  // distances[0]=far-right, [1]=right, [2]=center, [3]=left, [4]=far-left
  bool allBlocked = true;
  for (int i = 0; i < 5; i++) if (distances[i] > 10) { allBlocked = false; break; }

  if (allBlocked) {
    Serial.println("All blocked — reversing");
    reverse(); delay(600);
    turnRight(800);
    return;
  }

  float leftMax  = max(distances[3], distances[4]);
  float rightMax = max(distances[0], distances[1]);

  if (leftMax >= rightMax) {
    Serial.println("Turning LEFT");
    turnLeft(450);
  } else {
    Serial.println("Turning RIGHT");
    turnRight(450);
  }
}

void setup() {
  Serial.begin(9600);
  for (int p : {ENA, ENB, IN1, IN2, IN3, IN4}) pinMode(p, OUTPUT);
  pinMode(TRIG, OUTPUT); pinMode(ECHO, INPUT);
  scanServo.attach(SERVO_PIN);
  scanServo.write(90);
  delay(1000);
  Serial.println("Obstacle Avoider Ready.");
}

void loop() {
  float frontDist = ping();
  Serial.print("Front: "); Serial.print(frontDist, 0); Serial.println("cm");

  if (frontDist > SAFE_DIST) {
    forward();
  } else {
    stopMotors();
    delay(200);
    scanSurroundings();
    decideEscape();
  }
  delay(50);
}
