#include "SoundSensor.h"

SoundSensor sensor;

void setup() {
  Serial.begin(9600);
  sensor.begin();
}

void loop() {
  sensor.update();
  
  float avg = sensor.average();
  float min = sensor.min();
  float max = sensor.max();
  
  Serial.println("Avg: " + String(avg, 2) + " dBA");
  Serial.println("Min: " + String(min, 2) + " dBA");
  Serial.println("Max: " + String(max, 2) + " dBA");
  Serial.println("---");
  
  delay(1000);
}