# Garage-Door
ESP8266 Garage Door opener with Android App.

##Description

This is yet another Garage Door Opener using an ESP8266. It sets up a simple HTTP-like server that works with Fauxmo and Amazon Echo.

It is a modification of the basic Switch code to toggle the GPIO High and then Low. The server will set a GPIO pin which will be automatically reset after a short (500ms) pause. Typing http://server_ip/gpio/1 into your browser will set GPIO4 High (1), and then set it back to Low (0) to release the garage door switch. 

In previous iterations the toggle function was accomplished in the Android Garage Door Software, but this would not work with Echo, as it is only able to send on/off commands. 

The “Server_ip” is the IP address of the ESP8266 module, and will be printed to Serial when the module is connected to your computer (during initial programming). I included "Host Name" programming (set to “Garage Door”) for easy identification on the network so I can set a static IP on my network.

I made modifications to this software to interface a couple of HALL-Effect Sensors (Part Number: A3144E) as Status indicators. On the ESP8266 set pinmode(5, INPUT_PULLUP) (Status Open), and pinmode(12, INPUT_PULLUP) (Status Closed).

The lastest update includes Status Reporting. Typing http://server_ip/Status into your browser will return the status of the Garage Door based on the HALL Effect Sensors. This helps in the installation debug. Now you can check the alignment of the Magnets with the sensors. I plan to update the Android App as well so that you can check if you left the door open. It still only works if you are connected to your Home Wifi, but is handy for checking when you are in the backyard or can't see the door. It also helps in getting everything set-up.

I also implemented SMS notification of Garage Door Status using IFTTT Maker Recipe (Thanks to help from "The Hammer Project" http://hammerproject.com/post/130804023369/iot-intro-sms-me-when-i-leave-my-garage-door-open).  I'll add a description here of how to set this up later. The Code is set up to monitor two Hall-Effect Sensors and determine the time the door is open. It will send an SMS to your phone if it remains open for over an hour. You can change this time to suit your needs. This works even if you are not home, so you know if you left the door open, or if someone has opened your garage. 

I have included the "Over-The-Air (OTA) Update" code to allow for updating the ESP8266 without connecting a USB cable. Since this is installed overhead in the garage, it will be more convenient when I want to make changes. 

The Android App was written using the MIT App Inventor available on-line at: http://ai2.appinventor.mit.edu/; It provides the basic function of a garage door remote. You will need to be connected to your Home wifi, but if your house is like mine, my phone connects to the wifi when i pull up into the driveway. The latest version of the APP will provide Status Reporting of the door state as well. It works good for me on those days when I have been working in the garage, I can check just before I go to bed to make sure it is closed. The SMS message can do this as well, but if I am working out there I may ignore the message, so this way i can be sure.

You will need to import the .aia file above and edit the Web URL under the Blocks section to replace "Your Device IP" with the actual IP address of your garage door device.

##Parts List

-	(1) NodeMCU ESP8266  - Available from Amazon.com if you want it fast or from Alibaba.com if you want it cheap
-	(1) SPDT +5v Relay Module - I found mine on ebay for $1.08 ea.
-	(1) 2N2222A Transistor - The "A" is the metal can veriety, it helps with heat disipation
-	(1) 100K Resistor - 1/4 or 1/8 watt will do, not much current here
-	(2) A3144E Hall Effect Sensor - I found these on AliExpress 10 < $1.50 but they can be had at Amazon as well
-	(2) Magnets - I used the high power rare-earth type because I had them and it allowed me to have a little more distance between the
 	HALL-Effect sensor and the magnet
-	Solderable breadboard
-	Wire
-	(1) USB charger with cable - I used an old Phone charger, anything with ~.75 amp output will do


