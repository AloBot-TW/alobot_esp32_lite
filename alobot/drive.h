#ifndef DRIVE_H
#define DRIVE_H

enum DSTATE { D_BRAKE, D_DRIVE, D_ROTATION };

class Drive
{
  public:
    Drive();

    void startRotation(int l, int r, int d, int b);
    void rotation();
    void onRotation();
    void startDrive(int l, int r, int d);
    void drive();
    void onDrive();
    void startBrake();
    void onTimer();
    void brake(int duration);
    bool isBrake();

  private:
    int dd_duration;
    int dd_speed_l;
    int dd_speed_r;

    int ro_speed_l;
    int ro_speed_r;
    int ro_duration;
    int ro_break;

    bool brakeOn;

    DSTATE state;
};

#endif
