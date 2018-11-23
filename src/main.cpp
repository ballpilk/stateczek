
#include <Engine.hpp>
#include <Rudder.hpp>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>


ADC_MODE(ADC_VCC);
//WiFiServer server(80);
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
Pwm pwm;
Engine engine(pinMaszynyP, pinMaszynyT, pwm);
Rudder rudder(pinSteru, pwm);


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
  ser.on("/5/plus", [&](){   ser.send(200, "text/html", sendStatus());   engine.faster(); });
  ser.on("/5/minus", [&](){  ser.send(200, "text/html", sendStatus());    engine.slower();});
  ser.on("/5/reset", [&](){  ser.send(200, "text/html", sendStatus());    engine.stop();});
  ser.on("/4/plus", [&](){  ser.send(200, "text/html", sendStatus());    rudder.left();});
  ser.on("/4/minus", [&](){  ser.send(200, "text/html", sendStatus());    rudder.right();});
  ser.on("/4/max", [&](){  ser.send(200, "text/html", sendStatus());    rudder.maxRight();});
  ser.on("/4/minm", [&](){  ser.send(200, "text/html", sendStatus());    rudder.maxLeft();});
  ser.on("/4/reset", [&](){  ser.send(200, "text/html", sendStatus());    rudder.straight();});
  ser.on("/status", [&](){  ser.send(200, "text/html", sendStatus());});
  ser.on("/", [&](){  ser.send(200, "text/html", sendMain());});
  ser.on("", [&](){  ser.send(200, "text/html", sendMain());});
  ser.onNotFound([&](){  ser.send(200, "text/html", sendMain());});
}
void setup()
{
  Serial.begin(115200);
  Serial.println();
  WiFi.mode(WIFI_STA);


Serial.print("Configuring access point...");
/* You can remove the password parameter if you want the AP to be open. */
WiFi.config(local_IP, IPAddress(1,1,1,1), gateway, subnet);
WiFi.softAP("stateczek", "haseleczko");

IPAddress myIP = WiFi.softAPIP();
Serial.print("AP IP address: ");
Serial.println(myIP);
  delay(500);

  checkControlFunstions(server);
  server.begin();

}


void loop()
{
  server.handleClient();
  pwm.ping();
 }
