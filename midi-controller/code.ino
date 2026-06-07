// Requires Arduino Leonardo or Pro Micro
// Install: MIDIUSB library from Library Manager

#include <MIDIUSB.h>

const int NUM_POTS = 8;
const int NUM_BTNS = 8;

const int POT_PINS[NUM_POTS] = {A0, A1, A2, A3, A4, A5, A6, A7};
const int BTN_PINS[NUM_BTNS] = {2, 3, 4, 5, 6, 7, 8, 9};

// MIDI CC numbers per potentiometer
const byte CC_MAP[NUM_POTS] = {7, 74, 71, 91, 93, 94, 10, 1};
// MIDI notes per button (C2 = 36, chromatic up)
const byte NOTE_MAP[NUM_BTNS] = {36, 38, 40, 41, 43, 45, 47, 48};

const byte MIDI_CH = 0; // Channel 1 (0-indexed)
const int  ADC_HYSTERESIS = 3;

int prevPotVal[NUM_POTS];
bool prevBtnState[NUM_BTNS];
unsigned long lastBtnTime[NUM_BTNS];

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, (byte)(0xB0 | channel), control, value};
  MidiUSB.sendMIDI(event);
}

void noteOn(byte channel, byte note, byte velocity) {
  midiEventPacket_t event = {0x09, (byte)(0x90 | channel), note, velocity};
  MidiUSB.sendMIDI(event);
}

void noteOff(byte channel, byte note) {
  midiEventPacket_t event = {0x08, (byte)(0x80 | channel), note, 0};
  MidiUSB.sendMIDI(event);
}

void setup() {
  for (int i = 0; i < NUM_BTNS; i++) {
    pinMode(BTN_PINS[i], INPUT_PULLUP);
    prevBtnState[i] = true; // pulled high
    lastBtnTime[i] = 0;
  }
  for (int i = 0; i < NUM_POTS; i++) {
    prevPotVal[i] = analogRead(POT_PINS[i]) >> 3; // 10-bit → 7-bit
  }
}

void loop() {
  // --- Potentiometers → CC ---
  for (int i = 0; i < NUM_POTS; i++) {
    int raw = analogRead(POT_PINS[i]) >> 3; // Map 0-1023 to 0-127
    if (abs(raw - prevPotVal[i]) > ADC_HYSTERESIS) {
      controlChange(MIDI_CH, CC_MAP[i], (byte)raw);
      MidiUSB.flush();
      prevPotVal[i] = raw;
    }
  }

  // --- Buttons → Note On/Off ---
  for (int i = 0; i < NUM_BTNS; i++) {
    bool state = digitalRead(BTN_PINS[i]); // LOW = pressed (INPUT_PULLUP)
    if (state == prevBtnState[i]) continue;
    if (millis() - lastBtnTime[i] < 20) continue; // Debounce 20ms
    lastBtnTime[i] = millis();
    prevBtnState[i] = state;

    if (state == LOW) noteOn(MIDI_CH, NOTE_MAP[i], 100);
    else              noteOff(MIDI_CH, NOTE_MAP[i]);
    MidiUSB.flush();
  }
}
