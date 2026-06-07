#include <Wire.h>
#include <Adafruit_BME280.h>
#include <Adafruit_SSD1306.h>
#include <RTClib.h>
#include <SD.h>

#define SD_CS     4
#define OLED_ADDR 0x3C

Adafruit_BME280 bme;
Adafruit_SSD1306 display(128, 64, &Wire, -1);
RTC_DS3231 rtc;

const unsigned long LOG_INTERVAL = 10000UL; // 10 seconds
unsigned long lastLog = 0;

String currentFilename = "";

String buildFilename(DateTime dt) {
  char buf[16];
  sprintf(buf, "%04d%02d%02d.csv", dt.year(), dt.month(), dt.day());
  return String(buf);
}

void initFile(String fname) {
  if (!SD.exists(fname)) {
    File f = SD.open(fname, FILE_WRITE);
    if (f) { f.println("Timestamp,Temp_C,Humidity_%,Pressure_hPa,Altitude_m"); f.close(); }
  }
}

void logData(DateTime now, float t, float h, float p, float a) {
  String fname = buildFilename(now);
  if (fname != currentFilename) { currentFilename = fname; initFile(fname); }
  File f = SD.open(currentFilename, FILE_WRITE);
  if (!f) { Serial.println("SD write error"); return; }
  char line[80];
  sprintf(line, "%04d-%02d-%02d %02d:%02d:%02d,%.1f,%.1f,%.1f,%.1f",
    now.year(), now.month(), now.day(),
    now.hour(), now.minute(), now.second(),
    t, h, p, a);
  f.println(line);
  f.close();
}

void updateOLED(float t, float h, float p, float a, DateTime now) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(1);
  display.setCursor(0,0);  display.print(t,1); display.print("C  "); display.print(h,1); display.print("%");
  display.setCursor(0,12); display.print(p,1); display.print("hPa  "); display.print(a,0); display.print("m");

  display.drawFastHLine(0, 23, 128, SSD1306_WHITE);

  display.setCursor(0,27);
  char ts[20];
  sprintf(ts, "%02d/%02d/%04d %02d:%02d", now.day(), now.month(), now.year(), now.hour(), now.minute());
  display.print(ts);

  display.display();
}

void setup() {
  Serial.begin(9600);
  Wire.begin();

  if (!bme.begin(0x76)) { Serial.println("BME280 not found!"); while(1); }
  if (!rtc.begin())     { Serial.println("DS3231 not found!"); while(1); }
  if (rtc.lostPower())  { rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); }

  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  display.clearDisplay(); display.setTextSize(1); display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0); display.print("Initializing SD...");
  display.display();

  if (!SD.begin(SD_CS)) { Serial.println("SD init failed!"); }
  else { Serial.println("SD OK"); }

  DateTime now = rtc.now();
  currentFilename = buildFilename(now);
  initFile(currentFilename);

  Serial.println("Weather Station Ready. Logging every 10s.");
}

void loop() {
  if (millis() - lastLog < LOG_INTERVAL) return;
  lastLog = millis();

  float t = bme.readTemperature();
  float h = bme.readHumidity();
  float p = bme.readPressure() / 100.0F;
  float a = bme.readAltitude(1013.25);
  DateTime now = rtc.now();

  logData(now, t, h, p, a);
  updateOLED(t, h, p, a, now);

  Serial.print("T:"); Serial.print(t,1);
  Serial.print(" H:"); Serial.print(h,1);
  Serial.print(" P:"); Serial.print(p,1);
  Serial.print(" A:"); Serial.print(a,0);
  Serial.print(" → "); Serial.println(currentFilename);
}
