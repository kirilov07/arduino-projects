#include <MD_Parola.h>
#include <MD_MAX72XX.h>
#include <SPI.h>

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CS_PIN 10

MD_Parola display = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

String message = "Hello, World!";
uint8_t intensity = 5;

void setup() {
  Serial.begin(9600);
  display.begin();
  display.setIntensity(intensity);
  display.displayClear();
  display.setTextAlignment(PA_CENTER);
  Serial.println("LED Matrix Ready. Type a message to scroll. CMD: BRIGHT:0-15 STOP SCROLL");
  display.displayScroll(message.c_str(), PA_LEFT, PA_SCROLL_LEFT, 60);
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();
    if (cmd.startsWith("BRIGHT:")) {
      intensity = constrain(cmd.substring(7).toInt(), 0, 15);
      display.setIntensity(intensity);
      Serial.print("Brightness: "); Serial.println(intensity);
    } else if (cmd == "STOP") {
      display.displayClear();
      display.setTextAlignment(PA_CENTER);
      display.print(message.c_str());
    } else if (cmd == "SCROLL") {
      display.displayScroll(message.c_str(), PA_LEFT, PA_SCROLL_LEFT, 60);
    } else {
      message = cmd;
      display.displayScroll(message.c_str(), PA_LEFT, PA_SCROLL_LEFT, 60);
      Serial.print("Scrolling: "); Serial.println(message);
    }
  }

  if (display.displayAnimate()) {
    display.displayReset();
  }
}
