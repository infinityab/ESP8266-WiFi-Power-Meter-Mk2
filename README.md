# WiFi-Power-Meter-Mk1
ESP8266 WiFi SOC based Webserver to calculate accurate kilowatt consumption from digital electricity meters. The meters LED pulses are timed and converted into the Kilowatt/Hr equivalent and stored on the micro webserver as a web page. The data may then be retrieved via a tablet, smartphone, PC or Pi micro using a standard HTTP request. The message takes the form :-
  HTTP:// your-local-IP-address/gpio/0 0r 1/2/3/4 there are 5 messages at present in various forms of presentation.
  
  It has been tested on the Landis-Gyr EM5100 meter and works accurately and reliably although at present it wont indicate whether the power is being imported or exported, this is a function of this particular meter. A Mk2 wifi ESP meter is being tested which has a simple add-on module that connects to the existing Mk1 unit to indicate + or - power.
This project requires a light reader module ($1) and an ESP8266 module. The basic ESP8266-01 module is about $4 but this will also require an FTDI232 programmer module with some interconnecting wires and a voltage regulator. An easier choice is the ESP8266-12E Dev Kit module $8-12, this has all the required parts on the one module plus some additional ports.
NOTE that the ESP8266 runs on 3.3V NOT 5V. You will also need a suitable power source, a 9v-12v power supply is recommended. There are photographs of all the components in the images folder.

Bend the light detector so it will contact the flat meter surface and locate it over the pulsing LED and with power applied adjust until the LED on the module flashes in sympathy and temporarily just stick it in place with tape or bluetack. Atached the output and ground to the ESP8266 the output going to GPIO0. GPIO2 may be connected to an external LED if required via a suitable resistor (+-390ohms).

NOT FINISHED YET
