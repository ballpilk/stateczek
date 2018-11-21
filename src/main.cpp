#include <Arduino.h>
#include <ESP8266WiFi.h>
ADC_MODE(ADC_VCC);
WiFiServer server(80);

IPAddress myIP;
int pinMaszynyP(5);//maszyna+
int pinMaszynyT(16);
int pinSteru(4);//ster
int connectionLedPin(14);//D5
int maszyna(0), ster(0);
IPAddress local_IP(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
int vcc(0);

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
class Ster
{
public:
  Ster(int pin):pin_(pin){}
private:
  int pin_;
};
Maszyna silnik(pinMaszynyP, pinMaszynyT);
void setup()
{
  Serial.begin(115200);
  Serial.println();

  Serial.print("Setting soft-AP ... ");
  WiFi.softAPConfig(local_IP, gateway, subnet);
  bool result = WiFi.softAP("stateczek", "haseleczko");
  pinMode(pinSteru, OUTPUT);
  analogWriteFreq(300);
  pinMode(connectionLedPin, OUTPUT);
  if(result == true)
  {
    Serial.println("Ready");
  }
  else
  {
    Serial.println("Failed!");
  }
  myIP = WiFi.softAPIP();
  server.begin();
}
void sendResponseHeader(WiFiClient& client )
{
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println("Connection: close");
  client.println();
}
void sendStatus(WiFiClient& client)
{
  client.println("<!DOCTYPE html><html>");
  client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"></head>");
  client.println("<body onLoad=\"setTimeout(function(){ window.location.replace('http://192.168.1.1/status'); }, 2000);\"><p>Vcc: ");
  client.println(vcc);
  client.println("mV # ");
  client.println(WiFi.softAPgetStationNum());
  client.println("klientow # silnik: ");
  client.println(silnik.getSpeed());
  client.println(" # ster: ");
  client.println(ster);
  client.println(" </p></body></html> ");
}
void sendMainPage(WiFiClient& client)
{
  client.println("    <!DOCTYPE html><html>");
  client.println("  <head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  client.println("  <link rel=\"icon\" href=\"data:,\">");
  client.println("  <style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
  client.println("  .button { background-color: #195B6A; border: none; color: white; padding: 10px 20px;");
  client.println("  text-decoration: none; font-size: 15px; margin: 2px; cursor: pointer;}");
  client.println("  .button2 {background-color: #77878A;}</style></head>");
  client.println("  <body>");
  client.println("  <iframe src=/status  name=status height=30  width=100% ></iframe>");
  client.println("  <h1>Okrecik</h1>");
  client.println("  <p>Silnik");
  client.println("  </p>");
  client.println("  <p><a href=\"/5/plus\" target=status><button class=\"button\" formtarget=status>+</button></a></p>");
  client.println("  <p><a href=\"/5/reset\" target=status><button class=\"button button2\"  formtarget=status>Maszyna stop</button></a></p>");
  client.println("  <p><a href=\"/5/minus\" target=status><button class=\"button\" formtarget=status>-</button></a></p>");
  client.println("  <p>Ster  client.println(ster);</p>");
  client.println("  <p><a href=\"/4/max\" target=status><button class=\"button\" formtarget=status>--</button></a><a href=\"/4/plus\" target=status><button class=\"button\" formtarget=status>-</button></a>");
  client.println("  <a href=\"/4/reset\" target=status><button class=\"button button2\" formtarget=status>0</button></a>");
  client.println("  <a href=\"/4/minus\" target=status><button class=\"button\" formtarget=status>+</button></a><a href=\"/4/minm\" target=status><button class=\"button\" formtarget=status>++</button></a></p>");
  client.println("  </body></html>");
}
bool checkControlFunstions(const String& header)
{
  bool found = false;
  if (header.indexOf("GET /5/plus") >= 0)
  {
    found = true;
    silnik.faster();
  }
  else if (header.indexOf("GET /5/minus") >= 0)
  {
    found = true;
    silnik.slower();
  }
  else if (header.indexOf("GET /5/reset") >= 0)
  {
   found = true;
   silnik.stop();
  }
  else if (header.indexOf("GET /4/plus") >= 0)
  {
    found = true;
    ster = min(ster+32, 702);//990
    analogWrite(pinSteru, ster);
  }
  else if (header.indexOf("GET /4/max") >= 0)
  {
    found = true;
    ster = 702;
    analogWrite(pinSteru, ster);
  }
  else if (header.indexOf("GET /4/minus") >= 0)
  {
    found = true;
    ster = max(ster-32,216);//350
    analogWrite(pinSteru, ster);
  }
  else if (header.indexOf("GET /4/minm") >= 0)
  {
    found = true;
    ster = 216;
    analogWrite(pinSteru, ster);
  }
  else if (header.indexOf("GET /4/reset") >= 0)
  {
    found = true;
    ster = 440;
    analogWrite(pinSteru, ster);
  }
  else if (header.indexOf("GET /status") >= 0)
  {
    found = true;
  }
  return found;
}

void loop()
{
  if(WiFi.softAPgetStationNum() > 0)
  {
     digitalWrite(connectionLedPin,1);

  }
  else
  {
      digitalWrite(connectionLedPin, 0);
  }
  WiFiClient client = server.available();   // Listen for incoming clients

   if (client) {                             // If a new client connects,
     String header;
     Serial.println("New Client.");          // print a message out in the serial port
     String currentLine = "";                // make a String to hold incoming data from the client
     while (client.connected()) {            // loop while the client's connected
       if (client.available()) {             // if there's bytes to read from the client,
         char c = client.read();             // read a byte, then
         Serial.write(c);                    // print it out the serial monitor
         header += c;
         if (c == '\n') {                    // if the byte is a newline character
           if (currentLine.length() == 0) {
             vcc = ESP.getVcc();
             sendResponseHeader(client);
             if( checkControlFunstions(header))
                sendStatus(client);
            else
                sendMainPage(client);
             break;
           }
           else { // if you got a newline, then clear currentLine
             currentLine = "";
           }
         }
         else if (c != '\r') {  // if you got anything else but a carriage return character,
           currentLine += c;      // add it to the end of the currentLine
         }
       }
     }
     header = "";
     client.stop();
     Serial.println("Client disconnected.");
     Serial.println("");
   }


 }
