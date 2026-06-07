#include <Servo.h>

const int TRIG_PIN = 10;
const int ECHO_PIN = 11;
const int SERVO_PIN = 12;
const int MAX_DIST = 200; // cm

Servo radarServo;
int angle = 0;
int direction = 1; // 1=CW, -1=CCW

float readDistance() {
  digitalWrite(TRIG_PIN, LOW);  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms timeout
  if (duration == 0) return MAX_DIST;
  return duration * 0.0343 / 2.0;
}

void setup() {
  Serial.begin(9600);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  radarServo.attach(SERVO_PIN);
  radarServo.write(0);
  delay(500);
  Serial.println("Radar Scanner Ready.");
  Serial.println("Output format: ANGLE,DISTANCE  (for Processing sketch)");
  Serial.println("Format: angle,distance");
}

void loop() {
  radarServo.write(angle);
  delay(30); // Allow servo to reach position

  float dist = readDistance();
  dist = constrain(dist, 0, MAX_DIST);

  // CSV format compatible with Processing radar sketch
  Serial.print(angle);
  Serial.print(",");
  Serial.println(dist);

  angle += direction * 2; // 2 degree steps

  if (angle >= 180) { angle = 180; direction = -1; }
  if (angle <= 0)   { angle = 0;   direction =  1; }
}

/*
  Processing sketch to visualize this radar:
  Paste into Processing IDE (processing.org) and run:

  import processing.serial.*;
  Serial port;
  float dist, angle;
  void setup() {
    size(800, 400);
    port = new Serial(this, Serial.list()[0], 9600);
    port.bufferUntil('\n');
    background(0);
  }
  void serialEvent(Serial p) {
    String line = trim(p.readStringUntil('\n'));
    if (line == null) return;
    String[] parts = split(line, ',');
    if (parts.length < 2) return;
    angle = float(parts[0]); dist = float(parts[1]);
  }
  void draw() {
    fill(0, 15); rect(0, 0, width, height); // Trail
    int cx = width/2, cy = height - 20;
    float r = cy - 20;
    stroke(0, 255, 0); noFill();
    for (int i=1;i<=4;i++) { ellipse(cx, cy, r*i/2, r*i/2); }
    float px = cx + (dist/200.0)*r*cos(radians(180-angle));
    float py = cy - (dist/200.0)*r*sin(radians(180-angle));
    stroke(255, 0, 0); fill(255, 0, 0);
    ellipse((float)px, (float)py, 6, 6);
  }
*/
