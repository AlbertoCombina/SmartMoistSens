#include <Arduino.h>
#include "WiFiMulti.h"
#include "arduino_secrets.h"

// Creating a WiFi multi instance (This lib is built on top of WiFi.h)
WiFiMulti wifiMulti;

void setup() {
  Serial.begin(921600);
  pinMode(LED_BUILTIN, OUTPUT);

  // Adding an access point, that is your network 
  // (In theory if you have more then one you can specify SSID and PASS 
  // for all your network and the board will try to connect to all the networks if the other fails)
  wifiMulti.addAP(SECRET_SSID, SECRET_PASS);

  // Blocking connection check
  while(wifiMulti.run() != WL_CONNECTED){
    delay(100);
  }

  Serial.print("Connected to ");Serial.println(SECRET_SSID);
  Serial.print("Local IP address -> ");Serial.println(WiFi.localIP());
}

void loop() {
  // If the WiFi is connected the led is going to switch on
  digitalWrite(LED_BUILTIN, WiFi.status() == WL_CONNECTED);

}

