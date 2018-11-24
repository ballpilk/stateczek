#pragma once
#include <ESP8266WiFi.h>
#include "SoftPwm.hpp"
class Rudder
{
public:
  Rudder(int pin, Pwm& pwm): pwm_(pwm),pin_(pin)
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
    ster = 512;
    pwm_.setLevel(pin_, ster);
  }
  void maxLeft()
  {
    ster = 802;
    pwm_.setLevel(pin_, ster);
  }
  void maxRight()
  {
    ster = 240;
    pwm_.setLevel(pin_, ster);
  }
  int getRudder(){return ster;}
private:
  Pwm& pwm_;
  int ster;
  int pin_;
};
