#include <Arduino.h>
#include <Engine.hpp>
#include <Rudder.hpp>
#include <Ticker.h>
#include <ESP8266WiFiMulti.h>

int status = WL_IDLE_STATUS;
ESP8266WiFiMulti wifiMulti;      // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
void startWiFi()
{
  wifiMulti.addAP("pilotrc", "haseleczko");   // add Wi-Fi networks you want to connect to

  Serial.println("Connecting");
  while (wifiMulti.run() != WL_CONNECTED) {  // Wait for the Wi-Fi to connect
    delay(250);
    Serial.print('.');
  }
  Serial.println("\r\n");
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());             // Tell us what network we're connected to
  Serial.print("IP address:\t");
  Serial.print(WiFi.localIP());            // Send the IP address of the ESP8266 to the computer
  Serial.println("\r\n");
}
IPAddress server(192,168,1,1);  // pilot

WiFiClient client;
int pinMaszynyP(5);//D1 maszyna+
int pinMaszynyT(16);//D0
int pinSteru(10);//SD3
int connectionLedPin(12);//D6
int engineOnPin(4);//D2
unsigned int vcc(0);
unsigned long int lastStat = 0;
Ticker pwmTicker;
Pwm pwm;
Engine engine(pinMaszynyP, pinMaszynyT, pwm, engineOnPin);
Rudder rudder(pinSteru, pwm);


void ICACHE_RAM_ATTR onTimerISR(){
    pwm.ping();  //Toggle LED Pin
    timer1_write(500);
}

void setup()
{
  Serial.begin(9600);
  Serial.println();
  pinMode(A0, INPUT);
  pinMode(connectionLedPin, OUTPUT);
  startWiFi();

//  pwmTicker.attach_ms(1, &ping);
  timer1_attachInterrupt(onTimerISR);
  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
  timer1_write(500);
}
double getNumber(String buffer, char code)
{
  const char* buff = buffer.c_str();
  for(int i=0; i < buffer.length();++i)
  {
    if(buff[i] == code)
      return atof(&buff[i+1]);
  }
  return 0.;
}
int applyLimit(int throt, int lim)
{
  return throt < 0 ? max(throt, -lim) : min(throt, lim);

}
void loop()
{
  long unsigned int pingTime = micros();
  if (client.connect(server, 90)) {
    // Make a HTTP request:
    client.println(";");
    client.println();
    String response = client.readStringUntil(';');
    int throttle = getNumber(response, 'T');
    int limit = getNumber(response, 'L');
    int rud = getNumber(response, 'R');
    throttle = applyLimit(throttle, limit);
    engine.setSpeed(throttle);
    rudder.set(rud);
    response += ",";
    response += micros()-pingTime;
    response += ",";
    response += WiFi.RSSI();


    Serial.println(response);
    delay(20);
  }

}
