#pragma once
#include <Arduino.h>

class Rudder
{
public:
  Rudder(int pin):pin_(pin)
  {
    pinMode(pin_, OUTPUT);
    analogWriteFreq(300);
    straight();
  }
  void left()
  {
    ster = min(ster+32, 702);//990
    analogWrite(pin_, ster);
  }
  void right(){
    ster = max(ster-32, 216);//990
    analogWrite(pin_, ster);
  }
  void straight()
  {
    ster = 440;
    analogWrite(pin_, ster);
  }
  void maxLeft()
  {
    ster = 702;
    analogWrite(pin_, ster);
  }
  void maxRight()
  {
    ster = 216;
    analogWrite(pin_, ster);
  }
  int getRudder(){return ster;}
private:
  int ster;
  int pin_;
};
