#include <Arduino.h>
#include "tof.h"
#include "hwdef.h"

ToF::ToF() {
 
}

bool ToF::init() {
  Serial.println("*****ToF::init()");

  pinMode(VL53LOX_ShutdownPin, OUTPUT);
  Wire.begin(I2C_SDA, I2C_SCL);

  Serial.println("Adafruit VL53L0X XShut set Low to Force HW Reset");
  digitalWrite(VL53LOX_ShutdownPin, LOW);
  delay(100);
  digitalWrite(VL53LOX_ShutdownPin, HIGH);
  Serial.println("Adafruit VL53L0X XShut set high to Allow Boot");
  delay(100);

  sensor.setTimeout(500);

  if (!sensor.init())
    return false;

  //HIGH_ACCURACY
  //adjust incre timing budget (default is about 33 ms)
  sensor.setMeasurementTimingBudget(50000);

  //LONG_RANGE
  //adjust the return signal rate limit (default is 0.25 MCPS)
  sensor.setSignalRateLimit(2.5);

  //sensor.startContinuous();
  sensor.stopContinuous();

  conOn = false;

  return true;
}

void ToF::start() {
  sensor.startContinuous();
  conOn = true;
}

void ToF::stop() {
  sensor.stopContinuous();
  conOn = false;
}

int ToF::getDist() {
  return sensor.readRangeContinuousMillimeters();
}

