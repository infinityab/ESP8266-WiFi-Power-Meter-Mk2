# ESP8266 WiFi-Power-Meter-Mk2
ESP8266 WiFi SOC based Webserver Power Meter to calculate accurate kilowatt consumption from digital electricity meters. These meters emit LED pulses of usually 1000 pulses per hour per KW hr and these are detected, timed and converted into the Kilowatt/Hr equivalent and stored on the micro webserver as html data. A second infra-red detector is also used to differentiate whether outputing to the grid (excess solar) or inputing from the grid. The data may then be retrieved via a tablet, smartphone, PC or Pi micro using a standard HTTP request for display or further processing. 
The message takes the form :-
  
        http:// your-local-IP-address/gpio/0 or 1/2/3/4  

There are 5 messages at present in various forms to read power and a further 4 messages if you have installed the weather station option which will give you barometric pressure, temperature (2 sensors) and humidity.
 
In PHP the request would look like this :- e.g.  <?php    $json_string = file_get_contents("http://192.168.0.100/gpio/2");   ?>
where $json_string will then hold the power data.

It has been tested on the Landis-Gyr EM5100 electricity meter and works accurately and reliably.

REQUIREMENTS

<<<<<<< HEAD
The power meter consists of a light reader module ($1-50), a similar Infra Red module, an ESP8266-12e wifi module and a 5v power supply. There are a number of different ESP8266 modules but by far the easiest to use is the ESP8266-12 Dev Kit module (nodeMCU (LUA)) Aus$8-12, this has all the required parts to program the device on the one module plus some additional ports for other usage. The LUA language usually used with this module is not used in this project. There are a few of these Dev/Prototype versions around based on the -12 or -12E, the 12E has additional ports (pins) so is probably a better choice - see screenshots folder.
NOTE that the ESP8266 runs on 3.3V NOT 5V but the modules mentioned above have a built in regulator so while programming it the power comes from the +5v USB port. You will also need a suitable 5v - 9v power source when running on its own with a similar micro usb power plug. There are pics of all the components in the images folder.
=======
The power meter consists  of a standard light reader module ($1-50), an infra-red reading module, an ESP8266 wifi module and a 5v power supply is suggested. There are a number of different ESP8266 modules but by far the easiest to use is the ESP8266-12 Dev Kit module (nodeMCU (LUA)) Aus$8-$12, this has all the required functions to program or update the device on the one module plus some additional ports for other usage. The LUA language usually used with this module is not used in this project. This uses standard Arduino code (C++). There are a few of these Dev/Prototype versions around based on the -12 or -12E, the 12E has additional ports (pins) so is probably a better choice - see screenshots folder.
NOTE that the ESP8266 runs on 3.3V NOT 5V but the modules mentioned above have a built in regulator so while programming it the power comes from the +5v USB port. You will also need a suitable 5v - 9v power source when running on its own with a similar micro usb power lead. There are pics of all the components in the images folder.
>>>>>>> origin/master

This project uses the ESP8266 core library for Arduino which then allows you to program in Arduino code and use Arduino library and IDE. Download the ESP8266 Core library from https://github.com/esp8266/Arduino and the Arduino IDE from the Arduino website. Download this software (.ino file) and put it in your Arduino projects folders, download the include files and put them the a libraries folder. Set your SSID and password and meter pulse rate (default is 1000kw/hr), set the IP address and gateway etc. If you are using DHP comment out the 4 lines of code as indicated in the script/sketch. To program the module just upload the Power Meter software to your ESP8266 with the Arduino IDE. You will probably have to work through a few error breaks when compiling but these are nearly always just missing or mislocated include files.  

INSTALL

<<<<<<< HEAD
Bend the light detector so it will contact the flat meter surface and locate it over the pulsing LED and power the module with 3.3v from the ESP8266, adjust pot' until the LED on the module flashes in sympathy and temporarily just stick it in place with tape or bluetack. Do the same with the IR module except the IR LED has to be bent end on and shielded from any stray light. Attach the output (and ground) from light module to the ESP8266, the output to GPIO4 (D2), the IR module output to GPIO12 (D8) and GPIO5 (D1) may be connected to an external LED if required via a 470ohm resistor or thereabouts. If the pulses are registering OK with the ESP8266 the External LED will flash in sympathy. 
=======
Bend the light detector so it will contact the flat meter surface and locate it over the pulsing LED and power the module with 3.3v from the ESP8266, adjust pot' until the LED on the module flashes in sympathy and temporarily just stick it in place with tape or bluetack. Do the same with the infra-red module and shield around it to block any stray light coming in. Attach the output (and ground) from light module to the ESP8266, the output to GPIO4 (D2), and GPIO5 (D1) may be connected to an external LED if required via a 680ohm resistor or thereabouts. If the pulses are registering OK with the ESP8266 the External LED will flash in sympathy. 
>>>>>>> origin/master

 Initially set in debug mode and use the IDE serial monitor to watch module progress.

Ping the address of the ESP to ensure it is connected to wifi and make an HTTP request e.g http://192.168.0.100/gpio/0 and you should get an instant data return something like this :- Power Consumption : 3.435 KWs indicating your current power import or export. Try other messages /1/2/3/4 the replies are all slightly different. The Solar IOT uses the #4 command. When exporting power the value will be indicated without a sign, when importing power it will have a minus value.
