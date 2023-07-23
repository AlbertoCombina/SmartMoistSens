#include <Arduino.h>
#include "WiFi.h"
#include "arduino_secrets.h"

void setup() {
  Serial.begin(921600);
  pinMode(LED_BUILTIN, OUTPUT);

  WiFi.begin(SECRET_SSID,SECRET_PASS);
}

// Loop variables
bool isConnected = false;

void loop() {
  // If the WiFi is connected the led is going to switch on
  if(WiFi.status() == WL_CONNECTED && !isConnected){
    Serial.print("Connected to ");Serial.println(SECRET_SSID);
    Serial.print("Local IP address -> ");Serial.println(WiFi.localIP());
    digitalWrite(LED_BUILTIN, WiFi.status() == WL_CONNECTED);
    isConnected = true;
  }


// If not connected blink the led every second
  if(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(1000);
    isConnected = false;
  }
}

