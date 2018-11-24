#include <Engine.hpp>
#include <Rudder.hpp>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <Ticker.h>

ADC_MODE(ADC_VCC);
ESP8266WebServer server(80);

IPAddress myIP;
int pinMaszynyP(5);//maszyna+
int pinMaszynyT(16);
int pinSteru(10);//ster
int connectionLedPin(14);//D5
IPAddress local_IP(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
int vcc(0);
unsigned long int lastStat = 0;
Ticker pwmTicker;
Pwm pwm;
Engine engine(pinMaszynyP, pinMaszynyT, pwm);
Rudder rudder(pinSteru, pwm);

void ping()
{

}
ESP8266WiFiMulti WiFiMulti;

void sendResponseHeader(WiFiClient& client )
{
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println("Connection: close");
  client.println();
}
String sendStatus()
{
  String oss;
  oss += "<!DOCTYPE html><html>\
    <head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head>\
    <body onLoad=\"setTimeout(function(){ window.location.replace('/status'); }, 2000);\" style=\"margin:2px auto; font-size:12px; padding:2px\"><p align=center>Vcc: ";
    oss += vcc;
    oss += "mV # ";
    oss += WiFi.softAPgetStationNum();
    oss += "klientow # silnik: ";
    oss += engine.getSpeed();
    oss += " # ster: ";
    oss += rudder.getRudder();
    oss += " </p></body></html> ";
  Serial.print("s");
  return oss;
}
String sendMain()
{
  vcc = ESP.getVcc();
  String oss;
  oss += "    <!DOCTYPE html><html>\
    <head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\
    <link rel=\"icon\" href=\"data:,\">\
    <style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\
    .button { background-color: #195B6A; border: none; color: white; padding: 10px 20px;\
    text-decoration: none; font-size: 15px; margin: 2px; cursor: pointer;}\
    .button2 {background-color: #77878A;}</style></head><body>\
    <iframe src=/status  name=status height=60  width=100% ></iframe>\
    <h1>Okrecik</h1>\
    <p>Silnik </p>\
    <p><a href=\"/5/plus\" target=status><button class=\"button\" formtarget=status>+</button></a></p>\
    <p><a href=\"/5/reset\" target=status><button class=\"button button2\"  formtarget=status>Maszyna stop</button></a></p>\
    <p><a href=\"/5/minus\" target=status><button class=\"button\" formtarget=status>-</button></a></p>\
    <p>Ster</p>\
    <p><a href=\"/4/max\" target=status><button class=\"button\" formtarget=status>--</button></a><a href=\"/4/plus\" target=status><button class=\"button\" formtarget=status>-</button></a>\
    <a href=\"/4/reset\" target=status><button class=\"button button2\" formtarget=status>0</button></a>\
    <a href=\"/4/minus\" target=status><button class=\"button\" formtarget=status>+</button></a><a href=\"/4/minm\" target=status><button class=\"button\" formtarget=status>++</button></a></p>\
    </body></html>";
  Serial.print("m");
  return oss;
}
void checkControlFunstions(ESP8266WebServer& ser)
{
  ser.on("/5/plus", [&](){  engine.faster(); ser.send(200, "text/html", sendStatus());});
  ser.on("/5/minus", [&](){ engine.slower(); ser.send(200, "text/html", sendStatus());});
  ser.on("/5/reset", [&](){ engine.stop();   ser.send(200, "text/html", sendStatus());});
  ser.on("/4/plus", [&](){  rudder.left();   ser.send(200, "text/html", sendStatus());});
  ser.on("/4/minus", [&](){ rudder.right();  ser.send(200, "text/html", sendStatus());});
  ser.on("/4/max", [&](){  rudder.maxRight();ser.send(200, "text/html", sendStatus());});
  ser.on("/4/minm", [&](){ rudder.maxLeft(); ser.send(200, "text/html", sendStatus());});
  ser.on("/4/reset", [&](){rudder.straight();ser.send(200, "text/html", sendStatus());});
  ser.on("/status", [&](){ ser.send(200, "text/html", sendStatus());});
  ser.on("/", [&](){       ser.send(200, "text/html", sendMain());});
  ser.on("", [&](){        ser.send(200, "text/html", sendMain());});
  ser.onNotFound([&](){    ser.send(200, "text/html", sendMain());});
}
void ICACHE_RAM_ATTR onTimerISR(){
    pwm.ping();  //Toggle LED Pin
    timer1_write(600);//12us
}

void setup()
{
  Serial.begin(115200);
  Serial.println();
  WiFi.mode(WIFI_STA);
  Serial.print("Configuring access point...");
  WiFi.config(local_IP, IPAddress(1,1,1,1), gateway, subnet);
  WiFi.softAP("stateczek", "haseleczko");
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  delay(500);
  checkControlFunstions(server);
  server.begin();
//  pwmTicker.attach_ms(1, &ping);
  timer1_attachInterrupt(onTimerISR);
  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
  timer1_write(600); //120000 us
}
void loop()
{
  server.handleClient();
  
}
