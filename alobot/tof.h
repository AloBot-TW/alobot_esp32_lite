#ifndef TOF_H
#define TOF_H

#include <VL53L0X.h>

class ToF
{
  public:
    ToF();
    bool conOn;
    bool init();
    void start();
    void stop();
    int getDist();

  private:
    VL53L0X sensor;
   

};

#endif
