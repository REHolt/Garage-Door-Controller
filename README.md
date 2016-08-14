# Garage-Door
ESP8266 Garage Door opener with Android App
This is yet another Garage Door Opener using an ESP8266. It sets up a simple HTTP-like server that works with Fauxmo and Amazon Echo.
It is a modification of the basic Switch code to toggle the GPIO High and then Low
The server will set a GPIO pin which will be automatically reset after a short (500ms) pause
http://server_ip/gpio/1 will set the GPIO4 High (1), and then set back to Low (0) to release the garage door switch. 
In previous iterations the toggle function was accomplished in the Android Garage Door Software, 
but this would not work with Echo, as it is only able to send on/off commands. The Server_ip is the IP address of 
the ESP8266 module, and will be printed to Serial when the module is connected. This helps me identify the device so
I can set a static IP on my network.

I made modifications to this software to interface a couple of HALL-Effect Sensors (Part Number: A3144E) as Status indicators. 
On the ESP8266 set pinmode(5, INPUT_PULLUP) (Status Open). and pinmode(12, INPUT_PULLUP) (Status Closed).

Implemente SMS notification of Garage Door Status using IFTTT Maker Recipe. Code is set up to monitor the time the door is
open and send an SMS to my phone if it remains open for over an hour. 


I have included the _Over-The-Air(OTA) Update" code to allow for updating the ESP8266 without 
connecting a USB cable. Since this is installed overhead in the garage, it will be more conveinient when 
I want to make changes. Also, I included "Host Name" programming for easy identification on the network.

The Android App was writen using the MIT App Inventor available on-line at: http://ai2.appinventor.mit.edu/
You will need to edit the Web URL under the Blocks section to replace "Your Device IP" with the actual IP address of your garage door device.
