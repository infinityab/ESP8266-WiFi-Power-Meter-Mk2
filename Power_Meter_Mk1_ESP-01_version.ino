/*
 *  This sketch is for a simple but very accurate and reliable 2 port HTTP 
 *  webserver using the basic ESP8266-01 wifi soc module (note 3.3v)  
 *  and is designed to measure a digital electric meters 
 *  LED pulses and convert them into kilowatt power consumption.
 *  The data may then be retrieved via wifi by PC, tablet, 
 *  smartphone or micro as a normal web page request. There are 
 *  a number of different messages all slightly different to
 *  suit various requirements - pretty them up as you please.
 *  It can be configured in standard or debug mode.
 *  Debug additionally transmits serial process data and flashes the blue
 *  on-board LED. Power Light detector with 3.3v and connect output to GPIO0.
 *  An external LED may be added via a 470ohm resistor to port GPIO2  
 *  to indicate meter LED flashes are being processed although the
 *  light detector also has an LED indicating a meter pulse.
 *  You should add a reset button to this model to ensure it starts
 *  correctly as the 2 port pins have a dual purpose used also for 
 *  programming the module.
 *  The form of the request is such -
 *     http:// local server ip address/gpio/0 ... 1/2/3 .../8
 *     eg http://192.168.0.100/gpio/1
 *  The server_ip is the local IP address of the ESP8266 module, 
 *  and will be printed to Serial when the module is connected
 *  e.g. 192.168.0.100 required more if using DHP
 *  Power meter software Dave Clapham - infinityab56@gmail.com
 */

#include <ESP8266WiFi.h>

boolean debug = true;  // turn serial port debug OFF/ON
boolean pulsehigh = true;
const String st_html = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\n";
float power;
float eltimest;
float eltimeend;

// ******** USER ENTRIES *************
const char* ssid = "Band-NG"; //"Your network SSID";    // set your local network SSID
const char* password = "amersham"; //"SSID password";  
const unsigned int meter_pulses = 1000; // set for your meter eg 1000/KW, 800/Kw for Sprint meters

// Create an instance of the server and specify the port to listen on

WiFiServer server(80);

void setup() {
  pinMode(0, INPUT_PULLUP); // to try and ensure ESP8266 can start properly
  pinMode(2, INPUT_PULLUP); // with pulse input attached - see readme
  if (!debug) pinMode(1, OUTPUT); 
  delay(3000);
  if (debug) Serial.begin(115200);
  delay(500);
 
  pinMode(0, INPUT_PULLUP);    // prepare GPIO ports, set up pulse mode input
  pinMode(2, OUTPUT);
  if (!debug) digitalWrite(1,1);  // use TX LED as indicator
  digitalWrite(2, 0); // switch on external LED

  // Connect to WiFi network
  if (debug) {
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  }
  WiFi.begin(ssid, password);
  IPAddress ip(192, 168, 0, 116);   // * leave out the next 4 lines
  IPAddress gateway(192, 168, 0, 1);  // * if you are using DHP
  IPAddress subnet(255, 255, 255, 0); // *
  WiFi.config(ip, gateway, subnet); // *
  while (WiFi.status() != WL_CONNECTED) {
  delay(500);
  if (debug) Serial.print(".");
  }
  // Start the server
  if (debug) Serial.println("WiFi connected");
  server.begin();
  if (debug) {
    Serial.println("Server started");
    Serial.println(WiFi.localIP()); // Print the IP address
  }
}

void loop() {
  if (digitalRead(0) == LOW && pulsehigh) {
    pulsehigh = false;
    eltimeend = millis();
    power = 3600 * 1000 / (meter_pulses * (eltimeend - eltimest));
    eltimest = eltimeend;
    digitalWrite(2,0);  // LED indicator on this pin
    if (!debug) digitalWrite(1,0);  // use TX LED as indicator
    delay(100);  // debounce 100ms
    } else 
    {
      if (digitalRead(0) == HIGH && !pulsehigh)
        {
        pulsehigh = true;
        if (!debug) digitalWrite(1,1);  // use TX LED as indicator
        digitalWrite(2,1);  // LED indicator on this pin
        delay(100);  // debounce 100ms        
        }
    } 
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
      // Wait until the client sends some data
  int timer = 60; // but set a timeout
  Serial.println("new client");
  while(!client.available()){
    delay(50);
    yield();  // extra stack protection
    timer--;
    if (timer <= 0) {
      Serial.println("No Request");
      client.print(st_html + "No Request" + "</html>\n");
      client.flush();
      client.stop();
      return;  
    }
  }
  
  // Read the first line of the request
  String req = client.readStringUntil('\r');
  if (debug) Serial.println(req);
  client.flush();
  
 // Match the request
  int message;
  if (req.indexOf("/gpio/0") != -1)
    message = 0;
  else if (req.indexOf("/gpio/1") != -1)
    message = 1;
  else if (req.indexOf("/gpio/2") != -1)
    message = 2;
  else if (req.indexOf("/gpio/3") != -1)
    message = 3;  
  else if (req.indexOf("/gpio/4") != -1)
    message = 4;  
  else {
    if (debug) Serial.println("Invalid message request");
    client.println(st_html + "Invalid Message" + "</html>\n");
    client.stop();
    return;
  }
  // check GPIO - gpio/0 - /1/2/3/4 determines the message type returned
  // Prepare the response
String tx = "";
 switch (message) { // assemble the html data
  case 0:  
    tx += st_html + "Power Consumption : " + String(power) + "0 Kws"; 
    break; 
  case 1:  
    tx += st_html + "Power Consumption : " + String(power) + " Kws"; 
    break; 
  case 2:
    tx += st_html + String(power); 
    break;
  case 3:  
    tx += st_html + String(power) + "0";  
    break; 
  case 4:  
    tx += st_html + String(int(power * 1000)) + " Watts";  
    break; 
    default :
    tx += st_html + "Power Consumption : " + String(power) + "0 Kws"; 
    break;
 }
  tx += "</html>\n";
  client.println(tx); //   Send the response to the client

  if (debug) {
  Serial.println("Client disconnected");
  Serial.println(String(power) + "0 Kws");
  }
  // The client will be disconnected on function return
  // and 'client' object is destroyed
}

