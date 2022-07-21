#include <SoftwareSerial.h>
//#include <TM1637Display.h>
#include <TM1637TinyDisplay.h>

#define espResetPin 7
#define displayClk 9
#define displayDio 8
#define sensorTrig 11
#define sensorEcho 10

const unsigned long espTimeout = 20000L;

static float tankDiameter = 2.480;
static float tankVolume = 15000;
static float mountHeight = 3.280;

static float crossSectionArea = M_PI * pow((tankDiameter / 2.0), 2);
static float fullHeight = (tankVolume / 1000.0) / crossSectionArea;
static float deltaHeight = mountHeight - fullHeight;

SoftwareSerial EspSerial(2, 3); // RX, TX
//TM1637Display display(displayClk, displayDio);
TM1637TinyDisplay display(displayClk, displayDio);
uint8_t dots = 0b01000000;

//void setup() {}
//void loop() {}

//void setup() {
//  Serial.begin(9600);
//  while (!Serial);
//  EspSerial.begin(9600);
//  while (!EspSerial);
//
//  pinMode(espResetPin, OUTPUT);
//  digitalWrite(espResetPin, HIGH);
//  delay(10);
//
//  digitalWrite(espResetPin, LOW);
//  delay(2);
//  digitalWrite(espResetPin, HIGH);
//}
//
//void loop() {
//  if (Serial.available()) {
//    EspSerial.print(Serial.readStringUntil('\n'));
//  }
//
//  if (EspSerial.available()) {
//    Serial.write(EspSerial.read());
//  }
//}

void setup() {
  Serial.begin(9600);
  while (!Serial);
  EspSerial.begin(9600);
  while (!EspSerial);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  pinMode(espResetPin, OUTPUT);
  digitalWrite(espResetPin, HIGH);

  pinMode(sensorTrig, OUTPUT);
  pinMode(sensorEcho, INPUT);

  display.setBrightness(5);
  display.flipDisplay(true);
  display.clear();
}

void loop() {
  // fetch water reading
  float reading = getSensorValue();

  // display water reading
  displaySensorValue(reading);

  // wake esp and send data
  if (!wakeEsp()) {
    unoPrintln("failed to wake ESP");
  } else {
    sendReadingToEsp(String(reading, 2));

    // display for 10s or timeout
    displayEspOutput(espTimeout);
  }

//  delay(1000L * 60L * 30L);
//  delay(1000L * 60L * 15L);
  delay(1000L * 60L * 5L);
//  delay(1000L * 15L);
}

float getSensorValue() {
  int numSamples = 10;
  long totalTime = 0;
  for (int i = 0; i < numSamples; ++i) {
    totalTime += takeReading();
    delay(100);
  }

  float duration = totalTime / numSamples;

  float distance = duration * 0.343 / 2;
  // s = v * t (div 2 for there and back)
  // v = speed of sound = 0.343 mm/us

  // calibration 17/07/2022 21:35
  distance += 17.095;
  distance = distance / 0.99233;

  float waterMissingHeight = (distance / 1000.0) - deltaHeight;
  float waterMissingVolume = waterMissingHeight * crossSectionArea * 1000.0;
  float waterRemainingVolume = tankVolume - waterMissingVolume;

  String debug = "dist: ";
  debug += String(distance) + " mm";

  debug += ", miss_x: ";
  debug += String(waterMissingHeight) + " m";

  debug += ", miss_V: ";
  debug += String(waterMissingVolume) + " l";

  debug += ", rem_V: ";
  debug += String(waterRemainingVolume) + " l";

  unoPrintln(debug);

  return waterRemainingVolume;
}

long takeReading() {
  // Clears the sensorTrig
  digitalWrite(sensorTrig, LOW);
  delayMicroseconds(2);

  // Set HIGH for 10us
  digitalWrite(sensorTrig, HIGH);
  delayMicroseconds(10);
  digitalWrite(sensorTrig, LOW);

  // Read the sensorEcho, sound wave travel time in microseconds
  long duration = pulseIn(sensorEcho, HIGH);

  return duration;
}

void displaySensorValue(float value) {
  if (value >= 10000.0) {
    // want to display 12:34 for ~12340liters
    display.showNumberDec(value / 10.0, dots);
  } else {
    // want to display 4567 for ~4567liters
    display.showNumberDec(value);
  }
}

// ##############
// ESP functions
// ##############
bool wakeEsp() {
  digitalWrite(espResetPin, LOW);
  delay(2);
  digitalWrite(espResetPin, HIGH);

  return EspSerial.find("READY");
}

void sendReadingToEsp(String reading) {
  EspSerial.print("START");
  EspSerial.println(reading);
}

void displayEspOutput(unsigned long timeout) {
  bool ended = false;
  unsigned long now = millis();
  while (millis() < now + timeout) {
    String resp = EspSerial.readStringUntil('\n');
    Serial.println(resp);
    if (resp.indexOf("END") >= 0) {
      ended = true;
      break;
    }
  }

  unoPrintln(ended ? "[send] ended" : "[send] timed out");
}

// ##############
// String helpers
// ##############
void unoPrintln(String str) {
  Serial.println(String("[uno] ") + str);
}

void unoPrint(String str) {
  Serial.print(String("[uno] ") + str);
}

//void setup() {
//  display.setBrightness(4);
//
//  pinMode(sensorTrig, OUTPUT);
//  pinMode(sensorEcho, INPUT);
//
//  Serial.begin(9600);
//}
//
//void loop() {
//  float distance = getSensorValue();
//  displaySensorValue(distance);
//  delay(1000);
//}

//  if (resetAndWaitEsp()) {
//    Serial.print("Found\n");
//    writeSensorToEsp();
//
////    if (EspSerial.available()) {
////      String resp = EspSerial.readString();
////      Serial.print(resp + "\n");
////    }
//    unsigned long now = millis();
//    unsigned long timeout = 10000;
//    while (millis() < now + timeout) {
//      String resp = EspSerial.readStringUntil('\n');
//      Serial.println(resp);
//      if (resp.indexOf("END") >= 0) {
//        break;
//      }
//    }
//  } else {
//    Serial.print("Failed\n");
//  }
//
//  delay(300000);

//bool resetAndWaitEsp() {
//  digitalWrite(espResetPin, LOW);
//  delay(2);
//  digitalWrite(espResetPin, HIGH);
//
//  return EspSerial.find("Ready...");
//}
//
//void writeSensorToEsp() {
//  String sensorValue = "water_level=" + String(getSensorValue());
//  EspSerial.print(sensorValue + "\n");
////  Serial.print(sensorValue + "\n");
//  delay(2000);
//}
