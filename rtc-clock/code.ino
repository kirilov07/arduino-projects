#include <Wire.h>
#include <RTClib.h>
#include <LiquidCrystal_I2C.h>

RTC_DS3231 rtc;
LiquidCrystal_I2C lcd(0x27, 16, 2);

const char* days[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};

void printPad(int val) {
  if (val < 10) lcd.print("0");
  lcd.print(val);
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  lcd.init(); lcd.backlight();

  if (!rtc.begin()) {
    lcd.print("RTC not found!");
    while (1);
  }
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    Serial.println("RTC power lost — set to compile time.");
  }

  lcd.clear();
  Serial.println("RTC Clock Ready. Set time: SET:YYYY/MM/DD HH:MM:SS");
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n'); cmd.trim();
    if (cmd.startsWith("SET:")) {
      // Format: SET:2025/06/15 14:30:00
      String ts = cmd.substring(4);
      int yr   = ts.substring(0,4).toInt();
      int mo   = ts.substring(5,7).toInt();
      int dy   = ts.substring(8,10).toInt();
      int hr   = ts.substring(11,13).toInt();
      int mn   = ts.substring(14,16).toInt();
      int sc   = ts.substring(17,19).toInt();
      rtc.adjust(DateTime(yr, mo, dy, hr, mn, sc));
      Serial.println("Time updated.");
    }
  }

  DateTime now = rtc.now();

  // Row 0: day + date
  lcd.setCursor(0, 0);
  lcd.print(days[now.dayOfTheWeek()]);
  lcd.print(" ");
  printPad(now.day()); lcd.print("/");
  printPad(now.month()); lcd.print("/");
  lcd.print(now.year());

  // Row 1: time + temperature
  lcd.setCursor(0, 1);
  printPad(now.hour()); lcd.print(":");
  printPad(now.minute()); lcd.print(":");
  printPad(now.second());
  lcd.print(" ");
  lcd.print(rtc.getTemperature(), 1);
  lcd.print((char)223); lcd.print("C ");

  delay(500);
}
