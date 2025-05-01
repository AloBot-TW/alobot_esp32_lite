enum MSTATE { M_BRAKE, M_FWD, M_REV };

class Motor
{
  public:
    Motor(int dirPin, int pwmPin);

    void fwd(int speed);
    void rev(int speed);
    void brake();
    void drive(int data);
    bool isBrake();

  private:
	   int dirPin, pwmPin;
     MSTATE state;
};
