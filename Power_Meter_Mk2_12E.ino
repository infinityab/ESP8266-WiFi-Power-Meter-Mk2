/*
 *  This program is for a 3-in 1 unit. A Simple but accurate electricity meter reader, 
 *  an RC Wireless (315/433Mhz) socket switcher and a weather station
 *  combined using a multi-port webserver. It uses a ESP8266-12e 
 *  (dev-kit version) wifi soc module. It measures LED light pulses from 
 *  a digital electric meter and converts them into kilowatt power consumption.
 *  Two light modules are required, a standard light reader module and 
 *  an Infra Red reader module (on some meters).
 *  A standard 433Mhz wireless transmitter AND receiver and RC power sockets  are
 *  required. The receiver is used to intially discover the codes the remote 
 *  control unit is using so they can be incorporated into the software.
 *  The weather station is optional and measures temperature, barometric pressure
 *  and humidity. The Wireless relay point is optional, this would permit switching of 
 *  433Mhz (or 315Mhz) wireless sockets via WiFi command.
 *  Data for the above power and weather information may be retrieved via wifi 
 *  by PC, tablet, smartphone or micro Pi as a standard web page requests. 
 *  The form of power consumption requests is -
 *     http://local_server_ip_address/gpio/0 to 5
 *  A typical request from a Pi running PHP to request current power usage would be :
 *  $json_string = file_get_contents(http://192.168.0.100/gpio/4);  which would return the power in watts
 *  Wireless RC requests/commands are e.g. -   
 *  The server_ip is the local IP address of the ESP8266-12e module. 
 *
 *  Power meter software Dave Clapham - infinityab56@gmail.com
 */
 
#include <ESP8266WiFi.h>

//globals

const int aggmax = 3;  // // the number of aggreagate samples
const int $debounce = 150;  // debounce in ms
float aggpower[aggmax], aggtotal;  // = {  0, 0, 0, 0, 0, 0,};       // an array of power calculations
int aggposition;
boolean debug = false;  // turn serial port debug OFF/ON
boolean pulsehigh = true;
boolean irpulsehigh = false;
int gridpower;
const String st_html = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\n";
float power;
float eltimest;
float eltimeend;

// ******** USER ENTRIES *************
<<<<<<< HEAD:Power_Meter_Mk2_12E.ino
const char* ssid = "Band-NG-N3";    // set your local network SSID & password
const char* password = "amersham";  
=======
const char* ssid = "your SSID";    // set your local network SSID
const char* password = "your Password";  
>>>>>>> origin/master:Power_Meter_Mk1_12E_RCS_Ver.ino
const unsigned int meter_pulses = 1000; // set for your meter eg 1000/KW, 800/Kw for Sprint meters
 
// Create an instance of the server and specify the port to listen on

WiFiServer server(80);

void setup() {
  aggposition = 0;
  for (int sample = 0; sample < aggmax; sample++) {
  aggpower[sample] = 0;   // initialise aggregate power
  }
  if (!debug) pinMode(2, OUTPUT);
  pinMode(4, INPUT); // prepare GPIO ports, set up pulse mode input (D2)
  pinMode(5, OUTPUT); // External Meter Pulse LED (D1)
  pinMode(15, OUTPUT); // External IR Pulse LED (D8)
  pinMode(12, INPUT);  // Infra Red Test Light Detector (grid power use) (D6)

  delay(3000);
  if (debug) Serial.begin(9600);
  delay(500);
  if (!debug) digitalWrite(2,0);  // use TX LED as indicator
    else digitalWrite(15, 0); // switch on external LED

  // Connect to WiFi network
  if (debug) {
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
  }
  WiFi.begin(ssid, password);
  IPAddress ip(192, 168, 0, 116);   // * comment out the next 4 lines  
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

  if (digitalRead(12) == LOW && irpulsehigh) {   // looking for change IR LED
    irpulsehigh = false;
    gridpower = 5;  // need to make sure definately NOT on grid power
    if (debug) digitalWrite(15,0);  // LED indicator on this pin (D8)
        else digitalWrite(2,0);  // use TX IR LED as indicator
  } else {
    if (digitalRead(12) == HIGH && !irpulsehigh) {   // looking for change IR LED
      irpulsehigh = true;
      gridpower = 5;   // set grid power detector
      if (debug) digitalWrite(15,1);  // LED indicator on this pin (D8)
         else digitalWrite(2,1);  // use TX LED as indicator
    }
  }
  if (digitalRead(4) == LOW && pulsehigh) {
    pulsehigh = false;
    eltimeend = millis();
    gridpower--;
    if (gridpower > 0) {      // calculate current power
      power = (float)-1 * 3600 * 1000 / (meter_pulses * (eltimeend - eltimest)); 
      if (aggposition >= aggmax) {
        aggposition = 0;
        aggpower[aggposition] = (float)power;   // store power drawn from grid in circular buffer
        aggposition++;
        }
      else {
        aggpower[aggposition] = (float)power;
        aggposition++;
        }
      } else {   //  on solar power
        power = (float) 3600 * 1000 / (meter_pulses * (eltimeend - eltimest)); // solar power being drawn
        gridpower = 0;  // ensure it stays at zero
        if (aggposition >= aggmax) {
          aggposition = 0;
          aggpower[aggposition] = (float)power;    // storing power drawn from solar in circular buffer
          aggposition++;
          }
        else {
        aggpower[aggposition] = (float)power;
        aggposition++;
        }
      }    
    eltimest = eltimeend;
    digitalWrite(5,0);  // LED indicator on this pin (D1)
    delay($debounce);  // debounce
    } else {
    if (digitalRead(4) == HIGH && !pulsehigh) {
      pulsehigh = true;
      digitalWrite(5,1);  // LED indicator on this pin (D1)
      delay($debounce);  // debounce        
    } 
 }

  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
      // Wait until the client sends some data
  int timer = 30;
  Serial.println("new client");
  while(!client.available()){
  if (digitalRead(12) == LOW && irpulsehigh) {   // keep looking for change
    irpulsehigh = false;
    gridpower = 5;
  } else {
    if (digitalRead(12) == HIGH && !irpulsehigh) {   // looking for change
      irpulsehigh = true;
      gridpower = 5;
    }
  }
  delay(30);
  timer--;
  if (timer <= 0) {
    Serial.println("Bad Request");
    client.print(st_html + "Bad Request" + "</html>\n");
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
  else if (req.indexOf("/gpio/5") != -1)
    message = 5;   
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
    tx += st_html + "Power Consumption : " + String(power,3) + " Kws"; 
    break; 
  case 1:  
    tx += st_html + "Power Consumption : " + String((power*1000),0) + " Watts"; 
    break; 
  case 2:
    tx += st_html + String(power,3) + " Kws"; 
    break;
  case 3:  
    tx += st_html + String((power*1000),0) + " Watts";  
    break;
  case 4:  
    tx += st_html + String((power*1000),0);  
    break;   
  case 5:  
    aggtotal = 0;
    for (int sample = 0; sample < aggmax; sample++) {
       aggtotal += (float)aggpower[sample];   // total aggregate power samples
    }
    aggtotal = (float)aggtotal / aggmax; 
    tx += st_html + String((aggtotal*1000),0);  
    break;
   default :
    tx += st_html + "Invalid Request"; 
    break;
 }
  tx += "</html>\n";
  client.println(tx); //   Send the response to the client

  if (debug) {
  Serial.println("Client disconnected");
  Serial.println(tx);
  }
  // The client will be disconnected on function return
  // and 'client' object is destroyed
}

