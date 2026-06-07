#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET   -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define DHT_PIN 2
#define DHT_TYPE DHT22
DHT dht(DHT_PIN, DHT_TYPE);

const int HISTORY = 8;
float tempHistory[HISTORY] = {0};
float humHistory[HISTORY]  = {0};
int histIdx = 0;
int readings = 0;

unsigned long lastRead = 0;

void drawBarChart(float* data, int count, int x, int y, int w, int h, float minVal, float maxVal) {
  int barW = w / count;
  for (int i = 0; i < count; i++) {
    int barH = map(constrain((int)data[i], (int)minVal, (int)maxVal), (int)minVal, (int)maxVal, 1, h);
    display.fillRect(x + i * barW, y + h - barH, barW - 1, barH, SSD1306_WHITE);
  }
}

void setup() {
  Serial.begin(9600);
  dht.begin();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0,0); display.print("OLED Monitor Init...");
  display.display();
  delay(1000);
}

void loop() {
  if (millis() - lastRead < 2000) return;
  lastRead = millis();

  float temp = dht.readTemperature();
  float hum  = dht.readHumidity();
  if (isnan(temp) || isnan(hum)) { Serial.println("DHT read error"); return; }

  tempHistory[histIdx] = temp;
  humHistory[histIdx]  = hum;
  histIdx = (histIdx + 1) % HISTORY;
  readings = min(readings + 1, HISTORY);

  display.clearDisplay();

  // Top half: current values
  display.setTextSize(2);
  display.setCursor(0, 0); display.print(temp, 1); display.print("C");
  display.setCursor(68, 0); display.print(hum, 0); display.print("%");

  display.setTextSize(1);
  display.setCursor(0, 18); display.print("Temperature");
  display.setCursor(68, 18); display.print("Humidity");

  // Divider
  display.drawFastHLine(0, 27, 128, SSD1306_WHITE);

  // Bottom half: bar charts
  drawBarChart(tempHistory, HISTORY, 0,  30, 60, 34, 15, 40);
  drawBarChart(humHistory,  HISTORY, 68, 30, 60, 34, 20, 90);

  display.display();

  Serial.print("T:"); Serial.print(temp,1);
  Serial.print("°C  H:"); Serial.print(hum,0); Serial.println("%");
}
