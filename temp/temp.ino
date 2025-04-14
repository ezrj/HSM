#include <OneWire.h>
#include <DallasTemperature.h>

// Pin configuration for DS18B20 data pin
const int oneWireBus = 2;  // Digital pin connected to the Data pin of DS18B20

// Set up the OneWire bus
OneWire oneWire(oneWireBus);

// Pass OneWire reference to DallasTemperature library
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(9600);  // Start serial communication
  sensors.begin();     // Initialize the sensors
  Serial.println("DS18B20 Temperature Sensor Test");
}

void loop() {
  // Request temperature conversion
  sensors.requestTemperatures();
  
  // Read the temperature from the first sensor
  float temperatureC = sensors.getTempCByIndex(0);  // Get temperature in Celsius

  // Check if the reading is valid
  if (temperatureC != DEVICE_DISCONNECTED_C) {
    Serial.print("Temperature: ");
    Serial.print(temperatureC);
    Serial.println(" °C");
  } else {
    Serial.println("Error: Could not read temperature");
  }
  
  delay(1000);  // Delay for 1 second before next reading
}
