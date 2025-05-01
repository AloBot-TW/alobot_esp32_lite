#include <Arduino.h>

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#include "usled.h"
#include "hwdef.h"

#define NUMPIXELS 2 // Popular NeoPixel ring size

static Adafruit_NeoPixel pixels(NUMPIXELS, US_RGB, NEO_GRB + NEO_KHZ800);

const int TimeOut = 200;

UsLed::UsLed() {

#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif

  pixels.begin();

  timeoutCnt = 0;

}

bool UsLed::onLoop() {

  if (timeoutCnt > 10) {
      delay(100);
      return false;
  }

  pinMode(US_TRIG, OUTPUT);

  digitalWrite(US_TRIG, LOW); //關閉

  delay(1);

  digitalWrite(US_TRIG, HIGH);//啟動

  delay(1);

  digitalWrite(US_TRIG, LOW); //關閉

  pinMode(US_ECHO, INPUT);

  delay(1);

  unsigned long startTime = millis(); 

  float EchoTime = pulseIn(US_ECHO, HIGH, TimeOut*1000); //傳回時間

  unsigned long processTime = millis() - startTime;

  Serial.printf("pulseIn() %d\n", processTime);

  if (processTime == TimeOut) {
    timeoutCnt++;
    return false;
  }
   
  timeoutCnt = 0;

  float CMValue = EchoTime * 34 / 1000 / 2; //轉換成距離

  Serial.printf("CMValue=%f\n", CMValue);

  bool stop = false;
  if (CMValue != 0)
    stop = (CMValue < 30);

  int c = 50;

  pixels.clear();
  for(int i=0; i<NUMPIXELS; i++)
    pixels.setPixelColor(i, stop ? pixels.Color(0, c, 0) : pixels.Color(c, 0, 0));
  pixels.show();

  return stop;

}
