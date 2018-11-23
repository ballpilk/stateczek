#pragma once
#include <ESP8266WiFi.h>
#include "SoftPwm.hpp"
class Rudder
{
public:
  Rudder(int pin, Pwm& pwm):pin_(pin), pwm_(pwm)
  {
    pinMode(pin_, OUTPUT);
    pwm_.setFreq(300);
    straight();
  }
  void left()
  {
    ster = min(ster+32, 702);//990
    pwm_.setLevel(pin_, ster);
  }
  void right(){
    ster = max(ster-32, 216);//990
    pwm_.setLevel(pin_, ster);
  }
  void straight()
  {
    ster = 440;
    pwm_.setLevel(pin_, ster);
  }
  void maxLeft()
  {
    ster = 702;
    pwm_.setLevel(pin_, ster);
  }
  void maxRight()
  {
    ster = 216;
    pwm_.setLevel(pin_, ster);
  }
  int getRudder(){return ster;}
private:
  Pwm& pwm_;
  int ster;
  int pin_;
};
