// Obstacle Avoidance Robot — 2x IR sensors + L298N motor driver
// IR sensors are ACTIVE-LOW (LOW = obstacle detected)
// Wiring: IR_LEFT→2, IR_RIGHT→3
//         IN1-4→8,9,10,11  ENA→5  ENB→6

const int IR_LEFT  = 2;
const int IR_RIGHT = 3;

const int IN1 = 8,  IN2 = 9;   // Left motor
const int IN3 = 10, IN4 = 11;  // Right motor
const int ENA = 5,  ENB = 6;   // Speed (PWM, 0-255)

const int BASE_SPEED  = 160;
const int TURN_SPEED  = 180;

void motorLeft(int speed) {
  if (speed >= 0) { digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);  }
  else            { digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH); speed = -speed; }
  analogWrite(ENA, speed);
}

void motorRight(int speed) {
  if (speed >= 0) { digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);  }
  else            { digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH); speed = -speed; }
  analogWrite(ENB, speed);
}

void forward()   { motorLeft(BASE_SPEED);  motorRight(BASE_SPEED);  }
void backward()  { motorLeft(-BASE_SPEED); motorRight(-BASE_SPEED); }
void turnLeft()  { motorLeft(-TURN_SPEED); motorRight(TURN_SPEED);  }
void turnRight() { motorLeft(TURN_SPEED);  motorRight(-TURN_SPEED); }
void stopAll()   { motorLeft(0);           motorRight(0);           }

void setup() {
  Serial.begin(9600);
  pinMode(IR_LEFT,  INPUT);
  pinMode(IR_RIGHT, INPUT);
  int motorPins[] = {IN1, IN2, IN3, IN4, ENA, ENB};
  for (int p : motorPins) pinMode(p, OUTPUT);
  Serial.println("Obstacle Avoidance Robot Ready");
}

void loop() {
  bool leftBlocked  = (digitalRead(IR_LEFT)  == LOW);
  bool rightBlocked = (digitalRead(IR_RIGHT) == LOW);

  if (!leftBlocked && !rightBlocked) {
    forward();
    Serial.println("Forward");

  } else if (leftBlocked && !rightBlocked) {
    stopAll();
    delay(100);
    turnRight();
    Serial.println("Obstacle LEFT → Turn Right");
    delay(350);

  } else if (!leftBlocked && rightBlocked) {
    stopAll();
    delay(100);
    turnLeft();
    Serial.println("Obstacle RIGHT → Turn Left");
    delay(350);

  } else {
    // Both blocked
    stopAll();
    delay(100);
    backward();
    Serial.println("Both blocked → Reverse");
    delay(500);
    // Random turn direction
    if (random(2)) { turnLeft(); Serial.println("Random turn: Left"); }
    else           { turnRight(); Serial.println("Random turn: Right"); }
    delay(400);
  }

  delay(50);
}
