#include <Wire.h>
#include <Adafruit_BME280.h>
#include <BH1750.h>
#include <Adafruit_CCS811.h>
#include <Adafruit_SSD1306.h>

Adafruit_BME280 bme;
BH1750 lightMeter;
Adafruit_CCS811 ccs;
Adafruit_SSD1306 display(128, 64, &Wire, -1);

const int ALERT_LED  = 7;
const int BUZZER_PIN = 8;

const int ECO2_WARN  = 1000;
const int TVOC_WARN  = 500;

unsigned long lastRead  = 0;
unsigned long lastPage  = 0;
int page = 0;

// Rolling lux average
const int LUX_N = 20;
float luxBuf[LUX_N]; int luxIdx = 0;
float luxAvg = 0;

struct Readings { float temp, hum, press, alt, lux; uint16_t eco2, tvoc; };
Readings r;

String aqiLabel(uint16_t eco2) {
  if (eco2 < 600)  return "EXCELLENT";
  if (eco2 < 1000) return "GOOD";
  if (eco2 < 1500) return "MODERATE";
  return "POOR";
}

void drawPage0() { // Climate
  display.setTextSize(2);
  display.setCursor(0,0);  display.print(r.temp,1); display.print("C");
  display.setCursor(0,20); display.print(r.hum,0); display.print("%");
  display.setTextSize(1);
  display.setCursor(70,0);  display.print(r.press,0); display.print("hPa");
  display.setCursor(70,12); display.print(r.alt,0); display.print("m");
  display.setCursor(0,48); display.print("Climate");
}

void drawPage1() { // Light + Air Quality
  display.setTextSize(1);
  display.setCursor(0,0);  display.print("Lux:"); display.print(r.lux,0);
  display.setCursor(0,12); display.print("Avg:"); display.print(luxAvg,0);
  display.drawFastHLine(0, 23, 128, SSD1306_WHITE);
  display.setCursor(0,27); display.print("eCO2:"); display.print(r.eco2); display.print("ppm");
  display.setCursor(0,39); display.print("TVOC:"); display.print(r.tvoc); display.print("ppb");
  display.setCursor(0,51); display.print("AQI: "); display.print(aqiLabel(r.eco2));
}

void drawPage2() { // Mini bar chart: eCO2 trend not tracked yet, show summary
  display.setTextSize(1);
  display.setCursor(0,0);  display.print("Temp  Hum   Press");
  // Simple bar representation
  int tBar = map(constrain((int)r.temp, 0, 50), 0, 50, 0, 40);
  int hBar = map(constrain((int)r.hum,  0,100), 0,100, 0, 40);
  int pBar = map(constrain((int)(r.press-990), 0, 60), 0, 60, 0, 40);
  display.fillRect(0,  12, tBar, 12, SSD1306_WHITE);
  display.fillRect(44, 12, hBar, 12, SSD1306_WHITE);
  display.fillRect(88, 12, pBar, 12, SSD1306_WHITE);
  display.setCursor(0, 48); display.print(r.temp,1);
  display.setCursor(44,48); display.print(r.hum,0); display.print("%");
  display.setCursor(88,48); display.print(r.press,0);
}

void updateDisplay() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  if      (page == 0) drawPage0();
  else if (page == 1) drawPage1();
  else if (page == 2) drawPage2();
  display.display();
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  if (!bme.begin(0x76)) { Serial.println("BME280 fail"); while(1); }
  if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) { Serial.println("BH1750 fail"); while(1); }
  if (!ccs.begin()) { Serial.println("CCS811 fail"); while(1); }
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  pinMode(ALERT_LED, OUTPUT); pinMode(BUZZER_PIN, OUTPUT);
  while (!ccs.available()) delay(50); // Wait for CCS811 ready
  Serial.println("ts,temp,hum,press,alt,lux,eco2,tvoc,aqi");
}

void loop() {
  // Auto-cycle pages every 5s
  if (millis() - lastPage > 5000) { page = (page + 1) % 3; lastPage = millis(); }

  if (millis() - lastRead < 1000) { updateDisplay(); return; }
  lastRead = millis();

  r.temp  = bme.readTemperature();
  r.hum   = bme.readHumidity();
  r.press = bme.readPressure() / 100.0F;
  r.alt   = bme.readAltitude(1013.25);
  r.lux   = lightMeter.readLightLevel();

  // Compensate CCS811 with BME280 data
  ccs.setEnvironmentalData((uint8_t)r.hum, r.temp);
  if (ccs.available() && !ccs.readData()) {
    r.eco2 = ccs.geteCO2(); r.tvoc = ccs.getTVOC();
  }

  // Rolling lux average
  luxAvg = luxAvg - luxBuf[luxIdx] / LUX_N + r.lux / LUX_N;
  luxBuf[luxIdx] = r.lux; luxIdx = (luxIdx + 1) % LUX_N;

  // Alert
  bool alert = (r.eco2 > ECO2_WARN) || (r.tvoc > TVOC_WARN);
  digitalWrite(ALERT_LED, alert);
  if (alert) tone(BUZZER_PIN, 800, 200);

  // JSON serial output
  Serial.print("{\"ts\":"); Serial.print(millis()/1000);
  Serial.print(",\"temp\":"); Serial.print(r.temp,1);
  Serial.print(",\"hum\":"); Serial.print(r.hum,1);
  Serial.print(",\"press\":"); Serial.print(r.press,1);
  Serial.print(",\"alt\":"); Serial.print(r.alt,0);
  Serial.print(",\"lux\":"); Serial.print(r.lux,0);
  Serial.print(",\"eco2\":"); Serial.print(r.eco2);
  Serial.print(",\"tvoc\":"); Serial.print(r.tvoc);
  Serial.print(",\"aqi\":\""); Serial.print(aqiLabel(r.eco2)); Serial.println("\"}");

  updateDisplay();
}
