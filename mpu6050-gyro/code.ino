#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

float angleX = 0, angleY = 0;
unsigned long lastTime = 0;
const float ALPHA = 0.98; // Complementary filter weight

void setup() {
  Serial.begin(9600);
  Wire.begin();
  mpu.initialize();

  if (!mpu.testConnection()) {
    Serial.println("MPU6050 not found! Check wiring.");
    while (1);
  }

  Serial.println("MPU6050 Ready — Open Serial Plotter (115200 baud)");
  Serial.println("angleX:0,angleY:0"); // Init plotter labels
  Serial.begin(115200); // Switch to plotter baud rate
  lastTime = millis();
}

void loop() {
  int16_t ax, ay, az, gx, gy, gz;
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  float dt = (millis() - lastTime) / 1000.0;
  lastTime = millis();

  // Gyroscope angular rate (deg/s), sensitivity = 131 LSB/(deg/s)
  float gyroX = gx / 131.0;
  float gyroY = gy / 131.0;

  // Accelerometer angle
  float accelX = atan2(ay, az) * 180.0 / PI;
  float accelY = atan2(-ax, az) * 180.0 / PI;

  // Complementary filter: mostly gyro integration, corrected by accel
  angleX = ALPHA * (angleX + gyroX * dt) + (1.0 - ALPHA) * accelX;
  angleY = ALPHA * (angleY + gyroY * dt) + (1.0 - ALPHA) * accelY;

  // Serial Plotter format
  Serial.print("angleX:"); Serial.print(angleX, 2);
  Serial.print(",angleY:"); Serial.println(angleY, 2);

  delay(20); // ~50Hz
}
