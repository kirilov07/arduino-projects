#include <FastLED.h>

#define MIC_PIN  A0
#define LED_PIN  6
#define NUM_LEDS 20

CRGB leds[NUM_LEDS];
int noiseFloor = 512;

CRGB vuColor(int ledIndex) {
  float ratio = (float)ledIndex / NUM_LEDS;
  if (ratio < 0.5) return CRGB::Green;
  if (ratio < 0.75) return CRGB(255, 165, 0);
  return CRGB::Red;
}

void setup() {
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(80);

  // Calibrate noise floor over 100 samples
  long sum = 0;
  for (int i = 0; i < 100; i++) { sum += analogRead(MIC_PIN); delay(5); }
  noiseFloor = sum / 100;

  Serial.begin(9600);
  Serial.print("Noise floor calibrated: "); Serial.println(noiseFloor);
  Serial.println("Sound Reactive VU Meter Ready.");
}

void loop() {
  int sample = analogRead(MIC_PIN);
  int volume = abs(sample - noiseFloor);

  // Rolling noise floor adapts to ambient level
  noiseFloor = 0.99 * noiseFloor + 0.01 * sample;

  static int avgVolume = 0;
  avgVolume = 0.9 * avgVolume + 0.1 * volume;

  int activeLEDs = map(constrain(volume, 0, 300), 0, 300, 0, NUM_LEDS);

  // Smooth decay
  static int prevActive = 0;
  if (activeLEDs < prevActive) activeLEDs = max(activeLEDs, prevActive - 1);
  prevActive = activeLEDs;

  // Beat detection: sudden spike above 2.5× running average
  if (volume > avgVolume * 2.5 && volume > 80) {
    fill_solid(leds, NUM_LEDS, CRGB::White);
    FastLED.show();
    delay(40);
  }

  fill_solid(leds, NUM_LEDS, CRGB::Black);
  for (int i = 0; i < activeLEDs; i++) leds[i] = vuColor(i);
  FastLED.show();
  delay(10);
}
