// Frequency counter using Timer1 Input Capture (ICP1 = Pin 8)
// Signal generator on OC1A (Pin 9) via CTC mode

volatile uint32_t capture1 = 0, capture2 = 0;
volatile bool newCapture = false;
volatile uint8_t prescalerIdx = 0;

const uint8_t PRESCALERS[]   = {1, 8, 64, 256, 1024};
const uint16_t PRESCALE_BITS[] = {(1<<CS10), (1<<CS11), (1<<CS11)|(1<<CS10), (1<<CS12), (1<<CS12)|(1<<CS10)};

const int RANGE_LED = 6;
unsigned long lastGenFreq = 0;

ISR(TIMER1_CAPT_vect) {
  if (!newCapture) {
    if (capture1 == 0) { capture1 = ICR1; }
    else { capture2 = ICR1; newCapture = true; }
  }
}

void setupCapture(uint8_t prescIdx) {
  prescalerIdx = prescIdx;
  TCCR1A = 0;
  TCCR1B = (1 << ICES1) | (1 << ICNC1) | PRESCALE_BITS[prescIdx]; // Rising edge, noise cancel
  TIMSK1 = (1 << ICIE1); // Enable Input Capture interrupt
  capture1 = capture2 = 0;
  newCapture = false;
}

void setGenerator(unsigned long freq) {
  if (freq == 0) { TCCR1B &= ~(1<<WGM12); TCCR1A &= ~(1<<COM1A0); pinMode(9, INPUT); return; }
  // Use Timer1 CTC mode, OC1A toggle → freq = F_CPU / (2 * prescaler * (OCR1A+1))
  // Pick prescaler so OCR1A fits in 16 bits
  for (uint8_t ps = 0; ps < 5; ps++) {
    uint32_t ocr = (F_CPU / (2UL * PRESCALERS[ps] * freq)) - 1;
    if (ocr > 0 && ocr <= 65535) {
      TCCR1A = (1 << COM1A0);          // Toggle OC1A on compare match
      TCCR1B = (1 << WGM12) | PRESCALE_BITS[ps]; // CTC mode
      OCR1A = (uint16_t)ocr;
      TIMSK1 = 0;                       // Disable capture interrupt while generating
      pinMode(9, OUTPUT);
      lastGenFreq = freq;
      return;
    }
  }
  Serial.println("Frequency out of range");
}

void printFrequency(float hz) {
  if (hz >= 1e6)      { Serial.print(hz / 1e6, 3); Serial.println(" MHz"); }
  else if (hz >= 1e3) { Serial.print(hz / 1e3, 3); Serial.println(" kHz"); }
  else                { Serial.print(hz, 2);        Serial.println(" Hz"); }
}

void setup() {
  Serial.begin(9600);
  pinMode(8, INPUT);
  setupCapture(1); // Start with /8 prescaler
  Serial.println("Frequency Counter Ready. Input: Pin 8  Generator: Pin 9");
  Serial.println("Commands: GEN:1000  GEN:440  GEN:0");
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n'); cmd.trim();
    if (cmd.startsWith("GEN:")) {
      unsigned long f = cmd.substring(4).toInt();
      setGenerator(f);
      if (f > 0) { Serial.print("Generating: "); printFrequency(f); }
      else Serial.println("Generator stopped");
      // Re-enable capture after gen change
      if (f == 0) setupCapture(1);
    }
    return;
  }

  if (!newCapture) return;

  noInterrupts();
  uint32_t t1 = capture1, t2 = capture2;
  newCapture = false;
  capture1 = 0; capture2 = 0;
  interrupts();

  uint32_t period = (t2 >= t1) ? (t2 - t1) : (65535UL - t1 + t2);
  if (period == 0) return;

  float hz = (float)F_CPU / ((float)period * PRESCALERS[prescalerIdx]);

  // Auto-range
  if (hz > 50000 && prescalerIdx > 0)     { setupCapture(prescalerIdx - 1); return; }
  if (hz < 500   && prescalerIdx < 4)     { setupCapture(prescalerIdx + 1); return; }

  Serial.print("Frequency: "); printFrequency(hz);
  Serial.print("Period: ");
  if (hz > 0) { Serial.print(1.0/hz * 1e6, 2); Serial.println(" µs"); }

  digitalWrite(RANGE_LED, hz > 10000 ? HIGH : LOW);
}
