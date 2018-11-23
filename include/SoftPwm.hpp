#pragma once
#include <ESP8266WiFi.h>
#define MAX_PINS 17

struct Pin
{
  int level{0};
  bool active{false};
};
class Pwm
{
public:
  void setFreq(double freq)
  {
    dutyCycle = (long unsigned int)(1000000./freq);
    resolution = dutyCycle/1024;
  }
  void setLevel(int pinNo, unsigned int level)
  {
    if(level == 0)
    {
      pins[pinNo].active = false;
      digitalWrite(pinNo, LOW);
    }
    else
    {
      pins[pinNo].active = true;
      pins[pinNo].level = level;
    }
  }
  void ping()
  {
    long unsigned int time = micros();
    if(time  >= start + dutyCycle)
      start = time;
    for(int i = 0; i<MAX_PINS; ++i)
    {
      if( pins[i].active)
      {
          if(time < start + resolution*pins[i].level)
          {
            digitalWrite(i, HIGH);
          }
          else
          {
            digitalWrite(i, LOW);
          }
      }
    }

  }
private:
  long unsigned int resolution{1};
  long unsigned int dutyCycle{1};
  long unsigned int start{0};
  Pin pins[MAX_PINS];
};
