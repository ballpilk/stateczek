#pragma once
#include <ESP8266WiFi.h>
#include "SoftPwm.hpp"

class Maszyna{
public:
  Maszyna(int pinP, int pinT, Pwm& pwm)
  :pinP_(pinP), pinT_(pinT), pwm_(pwm)
  {
    pinMode(pinP_, OUTPUT);
    pinMode(pinT_, OUTPUT);
    stop();
  }
  void stop()
  {
    currSpeed_=0;
    setSpeed();
  }
  void faster(){
    currSpeed_ = min(currSpeed_ + 128, 1024);
    setSpeed();
  }
  void maxForward()
  {
    currSpeed_ = 1024;
    setSpeed();
  }
  void maxBackward()
  {
    currSpeed_ = -1024;
    setSpeed();
  }
  void slower(){
    currSpeed_ = max(currSpeed_ - 128, -1024);
    setSpeed();
  }
  int getSpeed(){return currSpeed_;}
private:
  void setSpeed()
  {
    if (currSpeed_ > 0)
    {
      pwm_.setLevel(pinP_, currSpeed_);
      pwm_.setLevel(pinT_, 0);
    }
    else if (currSpeed_ < 0)
    {
      pwm_.setLevel(pinT_, -currSpeed_);
      pwm_.setLevel(pinP_, 0);
    }
    else
    {
      pwm_.setLevel(pinP_, 0);
      pwm_.setLevel(pinT_, 0);
    }
  }
  Pwm& pwm_;
  int currSpeed_;
  int pinP_;
  int pinT_;
};
