#ifndef RING_H
#define RING_H



class Ring
{
  public:
    Ring();
    void init();
    void rainbow(int wait);
    void onDist(bool stop);
  
  private:
    bool fadeIn;
    uint8_t b;
    uint8_t durCnt;
   
};

#endif
