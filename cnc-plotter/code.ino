#include <Stepper.h>
#include <Servo.h>

// 28BYJ-48: 2048 steps/rev, 5.625°/step
const int STEPS_PER_REV = 2048;
Stepper stepX(STEPS_PER_REV, 2, 4, 3, 5);   // X axis ULN2003
Stepper stepY(STEPS_PER_REV, 6, 8, 7, 9);   // Y axis ULN2003
Servo penServo;
const int PEN_PIN = 10;
const int PEN_UP   = 70;
const int PEN_DOWN = 30;

// Steps per mm for your frame — calibrate by measuring actual movement
const float STEPS_PER_MM = 5.12;

long curX = 0, curY = 0; // current position in steps

void penUp()   { penServo.write(PEN_UP);   delay(150); }
void penDown() { penServo.write(PEN_DOWN); delay(150); }

// Bresenham line from (x0,y0) to (x1,y1) in step units
void lineTo(long x1, long y1) {
  long dx = abs(x1 - curX), dy = abs(y1 - curY);
  int sx = (x1 > curX) ? 1 : -1;
  int sy = (y1 > curY) ? 1 : -1;
  long err = dx - dy;

  while (curX != x1 || curY != y1) {
    long e2 = err * 2;
    if (e2 > -dy) { err -= dy; stepX.step(sx); curX += sx; }
    if (e2 <  dx) { err += dx; stepY.step(sy); curY += sy; }
  }
}

void moveTo(float xMM, float yMM) {
  long tx = (long)(xMM * STEPS_PER_MM);
  long ty = (long)(yMM * STEPS_PER_MM);
  lineTo(tx, ty);
}

void homeAxes() {
  lineTo(-curX, -curY); // Step back to origin
  curX = 0; curY = 0;
  penUp();
}

void parseGcode(String line) {
  line.trim();
  if (line.length() == 0 || line[0] == ';') { Serial.println("OK"); return; }

  float x = NAN, y = NAN;
  String cmd = "";

  // Extract command token
  int space = line.indexOf(' ');
  cmd = (space > 0) ? line.substring(0, space) : line;
  cmd.toUpperCase();

  // Parse X and Y
  int xi = line.indexOf('X'); if (xi >= 0) x = line.substring(xi+1).toFloat();
  int yi = line.indexOf('Y'); if (yi >= 0) y = line.substring(yi+1).toFloat();

  if (cmd == "G28") {
    homeAxes();
    Serial.println("OK");
  } else if (cmd == "G0") {
    penUp();
    if (!isnan(x) || !isnan(y)) {
      float tx = isnan(x) ? (float)curX / STEPS_PER_MM : x;
      float ty = isnan(y) ? (float)curY / STEPS_PER_MM : y;
      moveTo(tx, ty);
    }
    Serial.println("OK");
  } else if (cmd == "G1") {
    penDown();
    if (!isnan(x) || !isnan(y)) {
      float tx = isnan(x) ? (float)curX / STEPS_PER_MM : x;
      float ty = isnan(y) ? (float)curY / STEPS_PER_MM : y;
      moveTo(tx, ty);
    }
    Serial.println("OK");
  } else if (cmd == "M2" || cmd == "M30") {
    penUp();
    Serial.println("OK");
    Serial.println("Program end");
  } else {
    Serial.println("OK"); // Pass unknown commands silently
  }
}

void setup() {
  Serial.begin(9600);
  stepX.setSpeed(8);
  stepY.setSpeed(8);
  penServo.attach(PEN_PIN);
  penUp();
  Serial.println("CNC Plotter Ready. Send G-code. G28=home G0=rapid G1=draw M2=end");
}

void loop() {
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    parseGcode(line);
  }
}
