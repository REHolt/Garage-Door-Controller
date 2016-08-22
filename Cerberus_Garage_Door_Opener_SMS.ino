/*  This is the ESP8266 code that operates Cerberus the Garage door opener
 *   
 *  Note - In Arduino IDE set the "board" to "NodMCU 1.0 (ESP12E Module)"
 *  
 *  This sketch sets up a simple HTTP-like server that works with Fauxmo and Amazon Echo.
 *  
 *  It is a modification of the basic Switch code to toggle the GPIO High and then Low.  
 *  The server will set a GPIO pin which will be automatically reset after a short (500ms) pause.
 *  http://server_ip/gpio/1 will set the GPIO4 High (1), and then set back to Low (0) to release the garage door switch
 *  On the HiLetGo ESP8266 set pinmode(4,output)
 *  
 *  I just modified the code to include "Status" reporting. Typing  http://server_ip/Status into your browser
 *  will return the status information from the HALL Effect sensors. This helps in the installation debug. 
 *  Now you can check the alignment of the Magnets with the sensors. I will try to implement this into the 
 *  Android APP as a status button. This will let me check if I closed the door.
 *  
 *  On the Chines ESP8266 labeled LoLin set pinmode(4, output), actual output will be on pin labeled D2 
 *  In previous iterations (Greta) the toggle function was accomplished in the Android Garage Door Software (Greta)
 *  but this would not work with Echo, as it is only able to send on/off commands. The Server_ip is the IP address of 
 *  the ESP8266 module, and will be printed to Serial when the module is connected. I included "Host Name" programming, 
 *  this helps me identify the device so I can set a static IP on my network.
 *  
 *  I made modifications to this software to interface a couple of HALL-Effect Sensors (Part Number: A3144E) as Status indicators
 *  On the Chinese ESP8266 labeled Lolin set pinmode(5, INPUT_PULLUP). Actual input will be on pin labeled D1 (Status OPen)
 *  and pinmode(12, INPUT_PULLUP), Actual input will be on pin labeled D6 (Status Closed)
 *  
 *  I Implemente SMS notification of Garage Door Status using IFTTT Maker Recipe. Code is set up to monitor the time the door is
 *  open and send an SMS to my phone if it remains open for over an hour. 
 *  
 *  I have included the _Over-The-Air(OTA) Update" code to allow for updating the ESP8266 without 
 *  connecting a USB cable. Since this is installed overhead in the garage, it will be more conveinient when 
 *  I want to make changes. 
*/

#include <SimpleTimer.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266httpUpdate.h>
extern "C" {
#include "user_interface.h"
}
#include "Keys.h"  //includes the wifi credentials and IFTTT URL

//Garage Door Status variables
int Status_Open;
int Status_Closed;
int On_Status;
String Door_Status = "Unknown";
int val;

//Interrupt Definition
const byte interrupt1 = 12;
const byte interrupt2 = 5;
volatile byte state = LOW;

//Garage Door Open Timer setup
double openForTooLongInMins = 60;
int doorOpenedAtTimeInMills = 0;
int doorOpenDurationInSeconds = 0;
bool messageSentInThisOpening = false;
const int doorOpen = HIGH;
const int doorClosed = LOW;
SimpleTimer timer;

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

//-------------------------------------------------------------------------------
void setup() {

// Hostname defaults to esp8266-[ChipID]
wifi_station_set_hostname(Host_Name);

Serial.begin(115200);
delay(10);

Serial.println("Booting");
WiFi.mode(WIFI_STA);

pinMode(14, OUTPUT);

// prepare GPIO4 to control the door switch
pinMode(4, OUTPUT);
digitalWrite(4, 0);

// Prepare GPIO5 to monitor Status of the door
pinMode(interrupt1, INPUT_PULLUP);
attachInterrupt(digitalPinToInterrupt(interrupt1), Status, CHANGE);

// Prepare GPIO12 to monitor Status of the door
pinMode(interrupt2, INPUT_PULLUP);
attachInterrupt(digitalPinToInterrupt(interrupt2), Status, CHANGE);

// Connect to WiFi network
Serial.println();
Serial.println();
Serial.print("Connecting to ");
Serial.println(wifiCreds[0]);
Serial.println(Host_Name);

  WiFi.begin(wifiCreds[0], wifiCreds[1]);


while (WiFi.status() != WL_CONNECTED) {
delay(500);
// ESP.restart();
Serial.print(".");
}
Serial.println("");
Serial.println("WiFi connected");

// Start the server
server.begin();
Serial.println("Server started");

// Print the IP address
Serial.println(WiFi.localIP());

// print the host name
Serial.print("Host Name: ");
Serial.println(WiFi.hostname());

// Port defaults to 8266
// ArduinoOTA.setPort(8266);

// Hostname defaults to esp8266-[ChipID]
ArduinoOTA.setHostname(Host_Name);

// No authentication by default
//ArduinoOTA.setPassword((const char *)"1234");

ArduinoOTA.onStart([]() {
Serial.println("Start");
});

ArduinoOTA.onEnd([]() {
Serial.println("\nEnd");
});

ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
});

ArduinoOTA.onError([](ota_error_t error) {
Serial.printf("Error[%u]: ", error);
if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
else if (error == OTA_END_ERROR) Serial.println("End Failed");
});

//setup the OTA server
ArduinoOTA.begin();

Serial.println("Ready");
Serial.print("IP address: ");
Serial.println(WiFi.localIP());

//check Status of Garage Door at power-up
if (Door_Status = "Unknown") Status();
timer.setInterval(5000, checkOpen);
}
/*------------------------------------------------------------
* Interrupt Service Routine for Garage Door Status
* The routine is set-up to read two HALL-Effect sensors
* One (Status_Open) is triggered when the door is all the way open
* The second (Status_Closed) one is triggered when the door is all the way closed
* If they are both reading "Low" then there is a sensor error,
* since they are pulled up to 3.3volts and are pulled low when the magenet
* is present. The second one is probably over-kill, but it gives an indiction if the door
* is only partially open
*/

void Status() {
Status_Open = digitalRead(interrupt1);
Status_Closed = digitalRead(interrupt2);
switch (Status_Open) {
case 0:
{
switch (Status_Closed) {
case 0:
{
Door_Status = "Sensor Error";
}
break;
case 1: {
Door_Status = "Open";
}
break;
}
}
break;
case 1:
{
switch (Status_Closed) {
case 0:
{
Door_Status = "Closed";
}
break;
case 1: {
Door_Status = "Partially Open";
}
break;
}
}
}
}

/*------------------------------------------------------------
 * Door Monitoring and SMS message routines
 *  
*/
void resetDoorOpenCounter() {
doorOpenDurationInSeconds = 0;
messageSentInThisOpening = false;
}

void sendSms(String message) {
HTTPClient http;

http.begin(iftttMakerUrl, "A9 81 E1 35 B3 7F 81 B9 87 9D 11 DD 48 55 43 2C 8F C3 EC 87");

http.addHeader("content-type", "application/json");
int result = http.POST("{\"value1\":\"" + message + "\"}");

http.end();
}

void checkOpen() {
if ( digitalRead(interrupt2) == doorOpen ) {
doorOpenDurationInSeconds += 5;
}
if (digitalRead(interrupt2) == doorClosed) {
}

if ( messageSentInThisOpening == false &&
doorOpenDurationInSeconds > openForTooLongInMins * 60 ) {
String messageToSend = (String)"WARNING: your garage door has been open for more than " + openForTooLongInMins + " mins!";
sendSms(messageToSend);

doorOpenDurationInSeconds =0;
// todo: this does not know if it sent successfully. needs work
messageSentInThisOpening = true;
}
}


//--------------------------------------------------------------
void loop() {
timer.run();
ArduinoOTA.handle();

// Check if a client has connected
WiFiClient client = server.available();
if (!client) {
return;
}

// Wait until the client sends some data
Serial.println("new client");
while (!client.available()) {
delay(1);
}

// Read the first line of the request
String req = client.readStringUntil('\r');
Serial.println(req);
client.flush();

// Match the request
int val;
if (req.indexOf("/gpio/1") != -1){
val = 1;
digitalWrite(4, val);
delay (500);
val = 0;
digitalWrite(4, val);
client.flush();}
else if (req.indexOf("/Status") != -1){
val = 0;
client.flush();}
else {
val=0;
Serial.println("invalid request");
client.stop();
return;
}

// Prepare the response
String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n\r\n<html>\r\nGarage Door is now: ";
s += (Door_Status);
s += "</html>\n";

// Send the response to the client
client.print(s);
delay(1);
Serial.println("Client disonnected");

// The client will actually be disconnected
// when the function returns and 'client' object is detroyed

}
