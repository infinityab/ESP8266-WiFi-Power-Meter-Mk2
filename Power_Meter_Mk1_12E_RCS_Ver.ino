/*
 *  This program is for a 3-in 1 unit. A Simple but accurate electricity meter reader, 
 *  an RC Wireless (315/433Mhz) socket switcher and a weather station
 *  combined using a multi-port webserver. It uses a ESP8266-12e 
 *  (dev-kit version) wifi soc module. It measures LED light pulses from 
 *  a digital electric meter and converts them into kilowatt power consumption.
 *  Two light modules are required, a standard light reader module and 
 *  an Infra Red reader module (on some meters).
 *  A standard wireless transmitter AND receiver and RC power sockets  are
 *  required. The receiver is used to intially discover the codes the remote 
 *  control unit is using so they can be incorporated into the software.
 *  The weather station is optional and measures temperature, barometric pressure
 *  and humidity. The data may then be retrieved via wifi by PC, tablet, smartphone or micro 
 *  as a standard web page requests. RC switching is done in the same way. 
 *  The form of power consumption or weather requests is -
 *     http://local_server_ip_address/gpio/0 to 9
 *  A typical request from a Pi running PHP to request current power usage would be :
 *  $json_string = file_get_contents(http://192.168.0.100/gpio/4);  which would return the power in watts
 *  Wireless RC requests/commands are e.g. -   
 *      http://local_ip_address/sw1/0 or 1 0=OFF, 1=ON (or sw2, sw3,... etc)
 *  example - http://192.168.0.100/sw3/1
 *  The server_ip is the local IP address of the ESP8266-12e module, 
 *  and will be printed to Serial when the module is connected
 *  e.g. 192.168.0.100 required more if using DHP
 *  Power meter software Dave Clapham - infinityab56@gmail.com
 */

#include <ESP8266WiFi.h>
#include <RCSwitch.h>
#include <Adafruit_BMP085.h>
#include <UtilityFunctions.h>
#include <Wire.h>
#include <DHT.h>

#define I2C_SCL 1   // pin SD2 12      // Barometric Pressure Sensor (BMP085)
#define I2C_SDA 3  // pin SD3 - 13      // Barometric Pressure Sensor (BMP085)
#define DHTPIN 14       // pin D5 Temp/Humidity GPIO pin (DHT11)
#define DHTTYPE DHT11   // DHT 11 

//globals
int lc=0;
bool complete=false;
bool bmp085_present=true;
float bt,bp,ba,dhtt,dhth;
char btmp[20],bprs[20],balt[20],tin[20],tout[20],tatt[20],dhhumi[20],dhtemp[20]; 
uint32_t state=0;
char szT[30];

boolean debug = false;  // turn serial port debug OFF/ON
boolean pulsehigh = true;
boolean irpulsehigh = false;
int gridpower;
const String st_html = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\n";
float power;
float eltimest;
float eltimeend;
RCSwitch mySwitch = RCSwitch();

// ******** USER ENTRIES *************
const char* ssid = "Band-NG-N3";    // set your local network SSID
const char* password = "amersham";  
const unsigned int meter_pulses = 1000; // set for your meter eg 1000/KW, 800/Kw for Sprint meters

Adafruit_BMP085 bmp;
// OneWire  ds(DS18B20);  // on pin 5 for ds18b20
DHT dht(DHTPIN, DHTTYPE, 15);
// Create an instance of the server and specify the port to listen on

WiFiServer server(80);

void setup() {
  if (!debug) pinMode(2, OUTPUT);
  pinMode(4, INPUT); // prepare GPIO ports, set up pulse mode input (D2)
  pinMode(5, OUTPUT); // External Meter Pulse LED (D1)
  pinMode(15, OUTPUT); // External IR Pulse LED (D8)
  pinMode(16, OUTPUT);  // RC transmitter Din, *via resistors, TX could be < 12v (D0)
  pinMode(12, INPUT);  // Infra Red Test Light Detector (grid power use) (D6)
  mySwitch.enableTransmit(16);  // RC TX module is connected to ESP8266 PIN #16
  mySwitch.setPulseLength(250);  // Optional set pulse length for RC TX.
  mySwitch.setRepeatTransmit(3);  // Optional set number of transmission repetitions for RC TX.
  // mySwitch.setProtocol(2);  // Optional set protocol (default is 1, will work for most outlets)
   
  Wire.begin(I2C_SDA, I2C_SCL); // Setup BMP085 (i2c)
    if (!bmp.begin()) {
    Serial.println("No BMP085 sensor detected!");
    bmp085_present=false;
  }
  delay(3000);
  if (debug) Serial.begin(115200);
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
 // unsigned int ar = analogRead(A0);  // read analog input for power etc
 // delay(500);
 // Serial.println("Analogue Read Value : " + String(ar));
  if (digitalRead(12) == LOW && irpulsehigh) {   // looking for change IR LED
    irpulsehigh = false;
    gridpower = 5;  // need to make sure definately NOT on grid power
    if (debug) digitalWrite(15,0);  // LED indicator on this pin (D8)
        else digitalWrite(2,0);  // use TX IR LED as indicator
  } else {
    if (digitalRead(12) == HIGH && !irpulsehigh) {   // looking for change IR LED
      irpulsehigh = true;
      gridpower = 5;
      if (debug) digitalWrite(15,1);  // LED indicator on this pin (D8)
         else digitalWrite(2,1);  // use TX LED as indicator
    }
  }
  if (digitalRead(4) == LOW && pulsehigh) {
    pulsehigh = false;
    eltimeend = millis();
    gridpower--;
    if (gridpower > 0) {
      power = (float) -1 *3600 * 1000 / (meter_pulses * (eltimeend - eltimest)); 
      } else {
        power = (float) 3600 * 1000 / (meter_pulses * (eltimeend - eltimest)); //grid power being drawn
        gridpower = 0;  // ensure it stays at zero
      }    
    eltimest = eltimeend;
    digitalWrite(5,0);  // LED indicator on this pin (D1)
    delay(50);  // debounce 100ms
    } else {
    if (digitalRead(4) == HIGH && !pulsehigh) {
      pulsehigh = true;
      digitalWrite(5,1);  // LED indicator on this pin (D1)
      delay(50);  // debounce 100ms        
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
  delay(50);
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
   else if (req.indexOf("/gpio/5") != -1)
    message = 5;   
  else if (req.indexOf("/gpio/6") != -1)
    message = 6;
  else if (req.indexOf("/gpio/7") != -1)
    message = 7;  
  else if (req.indexOf("/gpio/8") != -1)
    message = 8; 
  else if (req.indexOf("/gpio/9") != -1)
    message = 9;   
  else if (req.indexOf("/sw1/0") != -1)
    message = 10;
  else if (req.indexOf("/sw1/1") != -1)
    message = 11;
  else if (req.indexOf("/sw2/0") != -1)
    message = 12;
  else if (req.indexOf("/sw2/1") != -1)
    message = 13;
  else if (req.indexOf("/sw3/0") != -1)
    message = 14;
  else if (req.indexOf("/sw3/1") != -1)
    message = 15;
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
    tx += st_html + "*" + String((power*1000),0) + "*";  
    break;
  case 6: //Read Baro Temperature
      if(bmp085_present) {
        bt = (bmp.readTemperature()); // degrees C 
        ftoa(bt,btmp, 1);
        tx += st_html + "Outside Temperature " + btmp +"C";  
      }
      else {
        strcpy(btmp,"not read");
      }
      break;
    case 7: //Read Baro Pressure
      if(bmp085_present) {
        bp = bmp.readPressure() / 99.62; // 3386.00 - note MUST add decimal places
        ftoa(bp,bprs, 1);
        tx += st_html + "Barometric Pressure " + bprs + "hPa";  
      }
      else {
        strcpy(btmp,"not read");
      }
      break;
    case 8:
      dhth = dht.readHumidity();
      ftoa(dhth,dhhumi, 1);
      tx += st_html + "Humidity " + dhhumi + "%";
      break;
    case 9:
      dhtt = (dht.readTemperature(true)-32)/1.8; // adjusted for C not F
      ftoa(dhtt,dhtemp, 1);
      tx += st_html + "Inside Temperature " + dhtemp + "C";
      state=0;
      break;
  
    case 10:  
    tx += st_html + "RC Switch 1 OFF";  
    mySwitch.send(3572860706, 32);  // OFF
    break; 
  case 11:  
    tx += st_html + "RC Switch 1 ON";  
    mySwitch.send(3572862764, 32);    // ON
    break;
  case 12:  
    tx += st_html + "RC Switch 2 OFF";  
    mySwitch.send(3572859686, 32);  // OFF
    break; 
  case 13:  
    tx += st_html + "RC Switch 2 ON";  
    mySwitch.send(3572861738, 32);    // ON
    break;
  case 14:  
    tx += st_html + "RC Switch 3 OFF";  
    mySwitch.send(3572860193, 32);  // OFF
    break; 
  case 15:  
    tx += st_html + "RC Switch 3 ON";  
    mySwitch.send(3572862254, 32);    // ON
    break;      
  default :
    tx += st_html + "Invalid Request"; 
    break;
 }
  tx += "</html>\n";
  client.println(tx); //   Send the response to the client

  if (debug) {
  Serial.println("Client disconnected");
  Serial.println(String(power,3) + " Kws");
  }
  // The client will be disconnected on function return
  // and 'client' object is destroyed
}

