# WiFi-Power-Meter-Mk1
ESP8266 WiFi SOC based Webserver Power Meter to calculate accurate kilowatt consumption from digital electricity meters. The meter LED pulses are timed and converted into the Kilowatt/Hr equivalent and stored on the micro webserver as a web page. The data may then be retrieved via a tablet, smartphone, PC or Pi micro using a standard HTTP request. The message takes the form :-
  HTTP:// your-local-IP-address/gpio/0 0r 1/2/3/4 there are 5 messages at present in various forms of presentation.
  
It has been tested on the Landis-Gyr EM5100 meter and works accurately and reliably although at present it wont indicate whether the power is being imported or exported, this is a function of this particular meter. A Mk2 wifi ESP meter is being tested which has a simple add-on module that connects to the existing Mk1 unit to indicate + or - power.

This project requires a light reader module ($1) and an ESP8266 module. There are a number of different ESP8266 modules but by far the easiest to use is the ESP8266-12 Dev Kit module (nodeMCU (LUA)) Aus$8-12, this has all the required parts to program the device on the one module plus some additional ports for other usage. The LUA language is not used in this project.

NOTE that the ESP8266 runs on 3.3V NOT 5V but the module mentioned above has a built in regulator so while programming it the power comes from the +5v USB port. You will also need a suitable 5v - 9v power source when running on its own. There are pics of all the components in the images folder.

This project uses the ESP8266 core for Arduino which then allows you to program in Arduino code and use Arduino library and IDE. Download the ESP8266 Core library from https://github.com/esp8266/Arduino and the Arduino IDE from the Arduino website. Flash the ESP8266 with the latest firmware version if required, just google on how to find and do this. You will need the firmware flasher utility and flash into 0x00000 location. 
Set your SSID and password and meter pulse rate (default is 1000kw/hr), set the IP address and gateway etc. if you are not using DHP and hold down the ESP flash button and press reset and upload the Power Meter software to your ESP8266 with the Arduino IDE.

Bend the light detector so it will contact the flat meter surface and locate it over the pulsing LED and with power applied adjust until the LED on the module flashes in sympathy and temporarily just stick it in place with tape or bluetack. Attach the output and ground to the ESP8266 the output going to GPIO4 (D2) and GPIO5 may be connected to an external LED if required via a 390ohm resistor or thereabouts. If the pulses are registering OK with the ESP8266 the External LED will flash in sympathy.

Ping the address of the ESP to ensure it is connected to wifi and make an HTTP request e.g http://192.168.0.100/gpio/0 and you should get an instant data return something like this :- Power Consumption : 3.435 KWs indicating your current power usage. Try other messages /1/2/3/4 the replies are all slightly different.
