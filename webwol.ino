// WebWol
// Author: Alfonso de Cala <alfonso@el-magnifico.org>
// Requires ESPAsyncWebServer and WakeOnLan

// Set up your network credentials
const char* ssid = "SSID";
const char* password = "PASSWORD";


#include <WiFi.h>
#include <WiFiUdp.h>
#include <WakeOnLan.h>
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"

WiFiUDP UDP;
WakeOnLan WOL(UDP);

// Set LED GPIO
const int ledPin = 2;
// Stores LED state
String State;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Replaces placeholder with LED state value
String processor(const String& var){
  return State;
}
 
void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());
 

  // Setup Wake on Lan
  WOL.setRepeat(3, 100); // Optional, repeat the packet three times with 100ms between. WARNING delay() is used between send packet function.
  WOL.calculateBroadcastAddress(WiFi.localIP(), WiFi.subnetMask()); // Optional  => To calculate the broadcast address, otherwise 255.255.255.255 is used (which is denied in some networks).
 

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    State="Ready";
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Route to set wake PC 1
  server.on("/wol1", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Waking up PC 1");

    digitalWrite(ledPin, HIGH);    
    const char *MACAddress = "01:23:45:67:89:AB";
    WOL.sendMagicPacket(MACAddress); 
    State="OK 1";
    request->send(SPIFFS, "/index.html", String(), false, processor);
    digitalWrite(ledPin, LOW);        

  });
  

  // Route to set wake PC 2
  server.on("/wol2", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Waking up PC 2");
    digitalWrite(ledPin, HIGH);    
    const char *MACAddress = "01:23:45:67:89:AB";
    const char *secureOn = "FE:DC:BA:98:76:54";
    WOL.sendSecureMagicPacket(MACAddress, secureOn); 
    State="OK 2";
    digitalWrite(ledPin, LOW);        
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  server.begin();
}
 




void loop()
{
}
