#include <Arduino.h>
#include "hwdef.h"
#include "motor.h"
#include "drive.h"

Motor motorL = Motor(A_1A, A_1B);
Motor motorR = Motor(B_1A, B_1B);

Drive::Drive() {
  state = D_BRAKE;
  brakeOn = false;
}

void Drive::startBrake() {
  brakeOn = true;
  brake(0);
}

void Drive::brake(int duration) {
  motorL.brake();
  motorR.brake();
  delay(duration);
  state = D_BRAKE;
}

void Drive::drive() {
  motorL.drive(dd_speed_l);
  motorR.drive(dd_speed_r);
}

void Drive::onDrive() {

  if (dd_duration == 0) {
    brake(0);
    return;
  }

  dd_duration--;
  drive();
}

void Drive::startDrive(int l, int r, int d) {

    brakeOn = false;

    if (state == D_ROTATION)
      return;

    state = D_DRIVE;

    dd_speed_l = l;
    dd_speed_r = r;
    dd_duration = d;

    onDrive();
}

int updateRoSpeed(int speed) {
  if (speed > 0x7F) {
    if (speed < 0xFF)
      speed++;
  } else {
    if (speed > 0)
      speed--;
  }
  return speed;
}

void Drive::rotation() {

  brake(ro_break);
  state = D_ROTATION;

  motorL.drive(ro_speed_l);
  motorR.drive(ro_speed_r);
/*
  Serial.print("ro_speed_l: ");
  Serial.println(ro_speed_l);
  Serial.print("ro_speed_r: ");
  Serial.println(ro_speed_r);
*/
  ro_speed_l = updateRoSpeed(ro_speed_l);
  ro_speed_r = updateRoSpeed(ro_speed_r);
}

void Drive::onRotation() {
  if (ro_duration > 0) {
    rotation();
    ro_duration--;
    if (ro_duration == 0) {
      brake(0);
      return;
    }
  }
}

void Drive::startRotation(int l, int r, int d, int b) {

  Serial.printf("startRotation() %d\n", state);

  brakeOn = false;

  if (state != D_BRAKE)
    return;

  state = D_ROTATION;

  ro_speed_l = l;
  ro_speed_r = r;
  ro_duration = d;
  ro_break = b;

  onRotation();
}

bool Drive::isBrake() {
  return (motorL.isBrake()&&motorR.isBrake());
}

void Drive::onTimer() {

  if (brakeOn) {
    if (isBrake())
      return;
    brake(0);
    return;
  }

  if (state == D_DRIVE)
    onDrive();
  else if (state == D_ROTATION)
    onRotation();

}
