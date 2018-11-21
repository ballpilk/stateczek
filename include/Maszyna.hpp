#pragma once
#include <Arduino.h>


class Maszyna{
public:
  Maszyna(int pinP, int pinT)
  :pinP_(pinP), pinT_(pinT)
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
      analogWrite(pinP_, currSpeed_);
      digitalWrite(pinT_, 0);
    }
    else if (currSpeed_ < 0)
    {
      analogWrite(pinT_, -currSpeed_);
      digitalWrite(pinP_, 0);
    }
    else
    {
      digitalWrite(pinP_, 0);
      digitalWrite(pinT_, 0);
    }
  }
  int currSpeed_;
  int pinP_;
  int pinT_;
};
