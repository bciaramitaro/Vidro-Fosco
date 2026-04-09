#include <ADCTouch.h>
#include <Servo.h>

// --- Servo Setup ---
Servo servo2; // continuous rotation servo

#define SERVO2_PIN 10

// --- LED Strip Pin ---
#define LED_STRIP_PIN 6

// --- Touch Pins ---
#define TOUCHPIN A0  // rocks (not used for action)
#define TOUCHPIN2 A1 // tree A (not used for action)
#define TOUCHPIN3 A2 // tree B triggers LED strip latch

// --- Touch Smoothing Setup ---
#define RESOLUTION 100
#define SMOOTH 100
float multiplier = 1.2;

// --- Touch Reading Buffers ---
int previousReadings[SMOOTH];
int currentIndex = 0;
int reading;
int state = 0;

int previousReadings2[SMOOTH];
int currentIndex2 = 0;
int reading2;
int state2 = 0;

int previousReadings3[SMOOTH];
int currentIndex3 = 0;
int reading3;
int state3 = 0;

// --- LED strip state flag ---
bool ledStripOn = false;

void setup() {
  servo2.attach(SERVO2_PIN);
  servo2.write(110); // Start continuous rotation servo

  pinMode(13, OUTPUT);            // Onboard LED for debug
  pinMode(LED_STRIP_PIN, OUTPUT); // LED strip output
  digitalWrite(LED_STRIP_PIN, LOW); // Start with LED strip OFF

  Serial.begin(9600);

  // Fill smoothing buffers
  for (int i = 0; i < SMOOTH; i++) {
    previousReadings[i] = ADCTouch.read(TOUCHPIN, RESOLUTION);
    previousReadings2[i] = ADCTouch.read(TOUCHPIN2, RESOLUTION);
    previousReadings3[i] = ADCTouch.read(TOUCHPIN3, RESOLUTION);
  }
}

void loop() {
  reading = ADCTouch.read(TOUCHPIN, RESOLUTION);
  reading2 = ADCTouch.read(TOUCHPIN2, RESOLUTION);
  reading3 = ADCTouch.read(TOUCHPIN3, RESOLUTION);

  // --- TouchPIN A0 ---
  if (reading > average(previousReadings)) {
    digitalWrite(13, HIGH); // debug LED
    state = 1;
  } else {
    digitalWrite(13, LOW);
    state = 0;
    updateBuffer(previousReadings, currentIndex, reading);
  }

  // --- TouchPIN A1 ---
  if (reading2 > average(previousReadings2)) {
    state2 = 1;
  } else {
    state2 = 0;
    updateBuffer(previousReadings2, currentIndex2, reading2);
  }

  // --- TouchPIN A2 (Triggers LED Strip once) ---
  if (reading3 > average(previousReadings3)) {
    state3 = 1;
    if (!ledStripOn) {
      digitalWrite(LED_STRIP_PIN, HIGH); // Turn on LED strip
      ledStripOn = true;                 // Latch ON
    }
  } else {
    state3 = 0;
    updateBuffer(previousReadings3, currentIndex3, reading3);
  }

  // Serial output for debugging
  Serial.print(state); Serial.print(" ");
  Serial.print(state2); Serial.print(" ");
  Serial.println(state3);
}

// --- Helper Functions ---
void updateBuffer(int* buffer, int& index, int value) {
  buffer[index] = value;
  index = (index + 1) % SMOOTH;
}

int average(int* buffer) {
  unsigned long sum = 0;
  for (int i = 0; i < SMOOTH; i++) sum += buffer[i];
  return (sum / SMOOTH) * multiplier;
}
