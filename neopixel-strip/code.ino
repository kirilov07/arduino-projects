#include <FastLED.h>

#define LED_PIN   6
#define NUM_LEDS  30
#define BRIGHTNESS 80

CRGB leds[NUM_LEDS];

String mode = "rainbow";
CRGB solidColor = CRGB::Blue;
uint8_t hueOffset = 0;

// --- Effects ---
void effectRainbow() {
  for (int i=0;i<NUM_LEDS;i++) leds[i] = CHSV((hueOffset + i * (255/NUM_LEDS)) % 256, 255, 255);
  hueOffset++;
}

void effectBreathing() {
  static uint8_t brightness = 0;
  static int8_t dir = 1;
  brightness += dir * 2;
  if (brightness >= 254 || brightness <= 1) dir = -dir;
  fill_solid(leds, NUM_LEDS, solidColor);
  FastLED.setBrightness(brightness);
}

void effectChase() {
  static int pos = 0;
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  for (int i=0;i<3;i++) leds[(pos + i) % NUM_LEDS] = solidColor;
  pos = (pos + 1) % NUM_LEDS;
}

void effectFire() {
  static byte heat[NUM_LEDS];
  for (int i=0;i<NUM_LEDS;i++) if (heat[i] > 0) heat[i]--;
  for (int i=NUM_LEDS-1;i>=2;i--) heat[i] = (heat[i-1]+heat[i-2]+heat[i-2])/3;
  if (random8() < 120) heat[random8(3)] = random8(160, 255);
  for (int i=0;i<NUM_LEDS;i++) {
    byte t = heat[i];
    leds[i] = (t < 85) ? CRGB(t*3, 0, 0) : (t < 170) ? CRGB(255, (t-85)*3, 0) : CRGB(255, 255, (t-170)*3);
  }
}

void effectMeteor() {
  static int pos = 0;
  fadeToBlackBy(leds, NUM_LEDS, 64);
  for (int i=0;i<6;i++) {
    int p = (pos - i + NUM_LEDS) % NUM_LEDS;
    leds[p] = CRGB(255, 255 - i * 30, 0);
  }
  pos = (pos + 1) % NUM_LEDS;
}

void setup() {
  Serial.begin(9600);
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  Serial.println("NeoPixel Strip Ready.");
  Serial.println("Modes: rainbow  breathing  chase  fire  meteor  solid");
  Serial.println("       BRIGHT:0-255  COLOR:R,G,B");
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n'); cmd.trim();
    if (cmd.startsWith("BRIGHT:")) {
      FastLED.setBrightness(constrain(cmd.substring(7).toInt(), 0, 255));
    } else if (cmd.startsWith("COLOR:")) {
      int r=0,g=0,b=0;
      sscanf(cmd.c_str()+6, "%d,%d,%d", &r, &g, &b);
      solidColor = CRGB(r, g, b); mode = "solid";
    } else {
      mode = cmd;
      FastLED.setBrightness(BRIGHTNESS);
    }
    Serial.print("Mode: "); Serial.println(mode);
  }

  if      (mode == "rainbow")   effectRainbow();
  else if (mode == "breathing") effectBreathing();
  else if (mode == "chase")     effectChase();
  else if (mode == "fire")      effectFire();
  else if (mode == "meteor")    effectMeteor();
  else if (mode == "solid")     fill_solid(leds, NUM_LEDS, solidColor);

  FastLED.show();
  delay(30);
}
