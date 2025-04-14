const int lightSensorPin = A0;

void setup() {
  Serial.begin(9600);
  Serial.println("Debugging analog light sensor...");
}

void loop() {
  int lightValue = analogRead(lightSensorPin);
  
  if (lightValue == 0) {
    Serial.println("Reading: 0. Check wiring or low light levels.");
  } else {
    Serial.print("Light value: ");
    Serial.println(lightValue);
  }

  delay(500);
}
