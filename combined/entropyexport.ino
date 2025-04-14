// same includes and setup as before, not tested
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define OV7670_ADDR 0x21
#define PIN_VSYNC 2
#define PIN_HREF 3
#define PIN_PCLK 4
#define PIN_XCLK 9

const int pixelPins[8] = {13, 5, 6, 7, 8, 10, 11, 12}; // D0–D7
const int tempPin = A0;
const int soundPin = A1;
const int lightPin = A2;

OneWire oneWire(tempPin);
DallasTemperature tempSensor(&oneWire);

void setup() {
  Serial.begin(115200);
  Wire.begin();
  for (int i = 0; i < 8; i++) pinMode(pixelPins[i], INPUT);
  pinMode(PIN_VSYNC, INPUT);
  pinMode(PIN_HREF, INPUT);
  pinMode(PIN_PCLK, INPUT);
  pinMode(PIN_XCLK, OUTPUT);
  setupXCLK();
  setupCamera();
  tempSensor.begin();
}

void loop() {
  byte entropy[32] = {0};

  int light = analogRead(lightPin);
  entropy[0] ^= light & 0xFF;
  entropy[1] ^= (light >> 2) & 0xFF;

  int sound = analogRead(soundPin);
  entropy[2] ^= sound & 0xFF;
  entropy[3] ^= (sound >> 3) & 0xFF;

  tempSensor.requestTemperatures();
  float tempC = tempSensor.getTempCByIndex(0);
  int tempScaled = (int)(tempC * 100);
  entropy[4] ^= tempScaled & 0xFF;
  entropy[5] ^= (tempScaled >> 8) & 0xFF;

  waitForVsync();
  delayMicroseconds(500);

  for (int row = 0; row < 10; row++) {
    waitForHref();
    delayMicroseconds(200);
    for (int col = 0; col < 10; col++) {
      waitForPclk(); delayMicroseconds(1);
      byte pixel = readPixelByte();
      waitForPclk(); delayMicroseconds(1);
      int idx = (row * 10 + col) % 32;
      entropy[idx] ^= pixel;
    }
    delayMicroseconds(50);
  }

  Serial.print("ENTROPY: ");
  for (int i = 0; i < 32; i++) {
    if (entropy[i] < 16) Serial.print("0");
    Serial.print(entropy[i], HEX);
  }
  Serial.println();

  delay(2000);
}

void waitForVsync() { while (digitalRead(PIN_VSYNC) == HIGH); while (digitalRead(PIN_VSYNC) == LOW); }
void waitForHref()  { while (digitalRead(PIN_HREF) == LOW); while (digitalRead(PIN_HREF) == HIGH); }
void waitForPclk()  { while (digitalRead(PIN_PCLK) == LOW); while (digitalRead(PIN_PCLK) == HIGH); }

byte readPixelByte() {
  byte val = 0;
  for (int i = 0; i < 8; i++) {
    val |= (digitalRead(pixelPins[i]) << i);
  }
  return val;
}

void setupXCLK() {
  TCCR1A = _BV(COM1A0);
  TCCR1B = _BV(WGM12) | _BV(CS10);
  OCR1A = 1;
}

void setupCamera() {
  writeReg(0x12, 0x80); delay(100);
  writeReg(0x12, 0x20);
  writeReg(0x11, 0x01);
  writeReg(0x0C, 0x00);
  writeReg(0x3E, 0x00);
  writeReg(0x40, 0x10);
}

void writeReg(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(OV7670_ADDR);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}
