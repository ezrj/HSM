#include <Wire.h>

// OV7670 I2C address (0x42 write >> 1)
#define OV7670_ADDR 0x21

// Pin definitions
const int PIN_VSYNC = 2;
const int PIN_HREF  = 3;
const int PIN_PCLK  = 4;
const int PIN_XCLK  = 9;

// Pixel data pins (D0–D7)
const int pixelPins[8] = {
  13, // D0
  5,  // D1
  6,  // D2
  7,  // D3
  8,  // D4
  10, // D5
  11, // D6
  12  // D7
};

void setup() {
  Serial.begin(115200);
  Wire.begin();

  // Setup control pins
  pinMode(PIN_VSYNC, INPUT);
  pinMode(PIN_HREF, INPUT);
  pinMode(PIN_PCLK, INPUT);
  pinMode(PIN_XCLK, OUTPUT);

  // Setup pixel data pins
  for (int i = 0; i < 8; i++) {
    pinMode(pixelPins[i], INPUT);
  }

  setupXCLK();     // Start XCLK
  setupCamera();   // Configure OV7670

  Serial.println("Camera ready...");
}

void loop() {
  waitForVsync();             // Start of a new frame
  delayMicroseconds(500);     // Skip some initial rows (tune this!)

  Serial.println("Frame Start");

  for (int row = 0; row < 10; row++) {
    waitForHref();            // Start of a line
    delayMicroseconds(200);   // Skip some pixels to reach center (tune this!)

    for (int col = 0; col < 10; col++) {
      waitForPclk();
      delayMicroseconds(1);   // Let signal settle
      byte y = readPixelByte();

      waitForPclk();          // Skip U byte
      delayMicroseconds(1);

      Serial.print(y);
      Serial.print("\t");     // Tab for spacing
    }

    Serial.println();         // End of row
    delayMicroseconds(50);    // Let HREF settle
  }

  Serial.println("Frame End\n");
  delay(1000); // Wait before reading the next frame
}


// ========== LOW-LEVEL FUNCTIONS ==========

void waitForVsync() {
  while (digitalRead(PIN_VSYNC) == HIGH);
  while (digitalRead(PIN_VSYNC) == LOW);
}

void waitForHref() {
  while (digitalRead(PIN_HREF) == LOW);
  while (digitalRead(PIN_HREF) == HIGH); // optional: ensure 1st line only
}

void waitForPclk() {
  while (digitalRead(PIN_PCLK) == LOW);
  while (digitalRead(PIN_PCLK) == HIGH);
}

byte readPixelByte() {
  byte val = 0;
  for (int i = 0; i < 8; i++) {
    val |= (digitalRead(pixelPins[i]) << i);
  }
  return val;
}

void setupXCLK() {
  pinMode(PIN_XCLK, OUTPUT);
  TCCR1A = _BV(COM1A0); // Toggle OC1A on compare match
  TCCR1B = _BV(WGM12) | _BV(CS10); // CTC mode, no prescaler
  OCR1A = 1; // 8 MHz = 16MHz / (2 * (1 + 1))
}

void setupCamera() {
  writeReg(0x12, 0x80); delay(100); // COM7: Reset
  writeReg(0x12, 0x20);             // COM7: YUV mode (Y channel = brightness)
  writeReg(0x11, 0x01);             // CLKRC: internal clock prescaler
  writeReg(0x0C, 0x00);             // COM3: no scaling
  writeReg(0x3E, 0x00);             // COM14: no DSP scaling
  writeReg(0x40, 0x10);             // COM15: full range output
}

void writeReg(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(OV7670_ADDR);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}
