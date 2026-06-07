const int SOIL_PIN   = A0;
const int RELAY_PIN  = 4;   // Active-LOW relay
const int LED_DRY    = 7;
const int LED_WET    = 6;

const int DRY_THRESHOLD  = 40;  // Below = dry, pump on
const unsigned long CHECK_INTERVAL = 30000UL;  // 30 seconds
const unsigned long PUMP_DURATION  = 3000UL;   // 3 seconds per pump cycle
const unsigned long COOLDOWN       = 60000UL;  // 60 seconds between pumps

unsigned long lastCheck = 0;
unsigned long lastPump  = 0;
bool pumping = false;
unsigned long pumpStart = 0;

int readMoisturePercent() {
  // Capacitive sensor: dry ~800 ADC, wet ~350 ADC
  int raw = analogRead(SOIL_PIN);
  return map(constrain(raw, 350, 850), 850, 350, 0, 100);
}

void pump(bool on) {
  digitalWrite(RELAY_PIN, on ? LOW : HIGH);  // Active-LOW
  digitalWrite(LED_DRY, on ? HIGH : LOW);
}

void setup() {
  Serial.begin(9600);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_DRY, OUTPUT);
  pinMode(LED_WET, OUTPUT);
  pump(false);
  Serial.println("Plant Watering System Ready");
  Serial.println("Moisture%  Status");
}

void loop() {
  unsigned long now = millis();

  // Stop pump after PUMP_DURATION
  if (pumping && (now - pumpStart >= PUMP_DURATION)) {
    pump(false);
    pumping = false;
    lastPump = now;
    Serial.println("Pump stopped.");
  }

  if (now - lastCheck < CHECK_INTERVAL) return;
  lastCheck = now;

  int moisture = readMoisturePercent();
  Serial.print(moisture); Serial.print("%  ");

  if (moisture >= DRY_THRESHOLD) {
    digitalWrite(LED_WET, HIGH); digitalWrite(LED_DRY, LOW);
    Serial.println("OK - Moist");
  } else {
    digitalWrite(LED_WET, LOW);
    Serial.print("DRY");
    if (!pumping && (now - lastPump >= COOLDOWN)) {
      pump(true);
      pumping = true;
      pumpStart = now;
      Serial.print(" -> Pumping...");
    } else if (pumping) {
      Serial.print(" (pumping)");
    } else {
      Serial.print(" (cooldown)");
    }
    Serial.println();
  }
}
