#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Adafruit_SSD1306.h>

Adafruit_INA219 ina219;
Adafruit_SSD1306 display(128, 64, &Wire, -1);

const int ALERT_LED = 7;
const float MAX_CURRENT_A = 2.0;
const float MAX_VOLTAGE_V = 26.0;

float whAccum = 0.0;
float peakPower = 0.0;
unsigned long lastSample = 0;
unsigned long startTime;

// Rolling average
const int AVG_N = 60;
float powerBuf[AVG_N];
int bufIdx = 0;
float rollingAvg = 0;

void drawOLED(float v, float mA, float mW, float wh, float avg) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0,0);  display.print(v,2); display.print("V");
  display.setCursor(68,0); display.print(mA,1); display.print("mA");

  display.setTextSize(1);
  display.setCursor(0,12); display.print(mW/1000.0,2); display.print("W");
  display.setCursor(68,12); display.print(wh,3); display.print("Wh");

  display.drawFastHLine(0, 23, 128, SSD1306_WHITE);

  display.setCursor(0,26); display.print("Avg:"); display.print(avg/1000.0,2); display.print("W");
  display.setCursor(0,36); display.print("Peak:"); display.print(peakPower/1000.0,2); display.print("W");

  unsigned long uptime = (millis() - startTime) / 1000;
  display.setCursor(0,48);
  display.print("Up:"); display.print(uptime/3600); display.print("h");
  display.print(((uptime%3600)/60)); display.print("m");

  display.display();
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  if (!ina219.begin()) { Serial.println("INA219 not found!"); while(1); }
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  pinMode(ALERT_LED, OUTPUT);
  startTime = millis();
  Serial.println("Power Monitor Ready.");
  Serial.println("Timestamp_s,Voltage_V,Current_mA,Power_mW,Energy_Wh");
}

void loop() {
  if (millis() - lastSample < 500) return;
  float dt = (millis() - lastSample) / 3600000.0; // hours
  lastSample = millis();

  float v    = ina219.getBusVoltage_V() + ina219.getShuntVoltage_mV() / 1000.0;
  float mA   = ina219.getCurrent_mA();
  float mW   = ina219.getPower_mW();

  // Trapezoidal Wh integration
  whAccum += mW * dt;
  if (mW > peakPower) peakPower = mW;

  // Rolling average
  rollingAvg = rollingAvg - powerBuf[bufIdx] / AVG_N + mW / AVG_N;
  powerBuf[bufIdx] = mW;
  bufIdx = (bufIdx + 1) % AVG_N;

  // Overcurrent / overvoltage alert
  bool alert = (mA > MAX_CURRENT_A * 1000) || (v > MAX_VOLTAGE_V);
  digitalWrite(ALERT_LED, alert ? HIGH : LOW);

  drawOLED(v, mA, mW, whAccum, rollingAvg);

  unsigned long ts = (millis() - startTime) / 1000;
  Serial.print(ts); Serial.print(",");
  Serial.print(v,3); Serial.print(",");
  Serial.print(mA,1); Serial.print(",");
  Serial.print(mW,1); Serial.print(",");
  Serial.println(whAccum,4);
}
