#include "motor.h"
#include <Arduino.h>
#include <driver/ledc.h>

const int freq = 1000;
const int res = 8; // 2^8 = 256

Motor::Motor(int dirPin, int pwmPin) {
  this->dirPin = dirPin;
  this->pwmPin = pwmPin;

  pinMode(dirPin, OUTPUT);
  ledcAttach(pwmPin, freq, res);

  state = M_BRAKE;

  brake();
}

void Motor::fwd(int speed) {
  if (speed > 255)
    speed = 255;
  digitalWrite(dirPin, LOW);
  ledcWrite(pwmPin, speed);
  state = M_FWD;
}

void Motor::rev(int speed) {
  if (speed > 255)
    speed = 255;
  digitalWrite(dirPin, HIGH);
  ledcWrite(pwmPin, 255-speed);
  state = M_REV;
}

void Motor::brake() {
   digitalWrite(dirPin, LOW);
   ledcWrite(pwmPin, 0);
   state = M_BRAKE;
}

bool Motor::isBrake() {
  return (state == M_BRAKE);
}

//00 (max backward) to 0x7F (stop) to ff (max forward)
void Motor::drive(int data) {
    if (data > 0x7F) {
      int speed = data - 0x7F;
      fwd(speed*2);
    } else if (data < 0x7F) {
      int speed = 0x7F - data;
      rev(speed*2);
    } else {
      brake();
    }
}
