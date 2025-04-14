const int envelopePin = A1;  // ENVELOPE connected to A0

void setup() {
  Serial.begin(9600);        // Start serial communication
  pinMode(envelopePin, INPUT);
}

void loop() {
  int soundLevel = analogRead(envelopePin);  // Read analog value (0–1023)
  Serial.println(soundLevel);                // Print value to Serial Monitor
  delay(100);                                // Small delay for readability
}