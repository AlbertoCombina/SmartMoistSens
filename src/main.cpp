#include <Arduino.h>
#include "WiFi.h"
#include "ssl_client.h"
#include "WiFiClientSecure.h"
#include "WebSockets.h"
#include "WebSocketsClient.h"
#include "arduino_secrets.h"
#include <ArduinoJson.h>

#define WS_HOST "" 
#define WS_PORT 443
#define WS_URL ""

#define JSON_DOC_SIZE 2048
#define MSG_SIZE 256

bool isConnected = false;
WebSocketsClient wsClient;

void sendErrorMessage(const char * error){
  char msg[MSG_SIZE];
  sprintf(msg, "\"action\":\"msg\",\"type\":\"error\",\"body\":\"%s\"", error);
  wsClient.sendTXT(msg);
}

void sendOkMessage(){
  wsClient.sendTXT("\"action\":\"msg\",\"type\":\"status\",\"body\":\"ok\"");
}

uint8_t toMode(const char * val){
  // Convert the pin mode coming from the command to the proper constant 
  if(strcmp(val, "output") == 0){
    return OUTPUT;
  } else if(strcmp(val, "input_pullup") == 0){
    return INPUT_PULLUP;
  } else{
    return INPUT;
  }
}

void handleMessage(uint8_t * payload){
  StaticJsonDocument<JSON_DOC_SIZE> doc;
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, payload);

  // Test if parsing succeeds.
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    sendErrorMessage(error.c_str());
    return;
  }

  // Checking that the the type field of the json is not empty 
  if(!doc["type"].is<const char *>()){
    sendErrorMessage("Invalid message type format");
    return;
  }

  // If the type is identical to cmd
  if(strcmp(doc["type"], "cmd") == 0){

    // The body is a json, so it is checking the type
    if(doc["body"].is<JsonObject>()){
      sendErrorMessage("Invalid command body format");
      return;
    }

    // Command #1 - Setting the pinMode
    if(strcmp(doc["body"]["type"],"pinMode") == 0){
      pinMode(doc["body"]["pin"], toMode(doc["body"]["mode"]));
      sendOkMessage();
      return;
    }

    // Command #2 - DigitalWrite
    if(strcmp(doc["body"]["type"],"digitalWrite") == 0){
      digitalWrite(doc["body"]["pin"], toMode(doc["body"]["value"]));
      sendOkMessage();
      return;
    }

    // Command #3 - DigitalRead
    if(strcmp(doc["body"]["type"],"digitalRead") == 0){
      int digReadValue = digitalRead(doc["body"]["pin"]);
      char msg[MSG_SIZE];
      sprintf(msg, "\"action\":\"msg\",\"type\":\"output\",\"body\":%d", digReadValue);
      wsClient.sendTXT(msg);
      return;
    }

    // Command # DEFAULT
    sendErrorMessage("Unsupported command type");
    return;
  }

  sendErrorMessage("Unsupported message type");
  return;
}

void onWSEvent(WStype_t type, uint8_t * payload, size_t length){
  // type = type of event
  // payload = anything that comes with the event
  // lenght = lenght of the payload

  switch(type){
    case WStype_ERROR:
      break;
    case WStype_DISCONNECTED:
      Serial.println("WS DISCONNECTED from the server! ");
      break;
    case WStype_CONNECTED:
      Serial.println("WS CONNECTED to the server! ");
      break;
    case WStype_TEXT:
      Serial.printf("WS Message: %s\n", payload);
      handleMessage(payload);
      break;
    case WStype_BIN:
      break;
    case WStype_FRAGMENT_TEXT_START:
      break;
    case WStype_FRAGMENT_BIN_START:
      break;
    case WStype_FRAGMENT:
      break;
    case WStype_FRAGMENT_FIN:
      break;
    case WStype_PING:
      break;
    case WStype_PONG:
      break;
    default:
      break;
  }
}

void setup() {
  Serial.begin(921600);
  pinMode(LED_BUILTIN, OUTPUT);

  WiFi.begin(SECRET_SSID,SECRET_PASS);
  Serial.print("\nTrying to connect to ");Serial.println(SECRET_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(200);
  }
  // Connection succeded
  isConnected = true;
  Serial.print("\nCONNECTED!!!");Serial.print("\tLocal IP address -> ");Serial.println(WiFi.localIP());
  digitalWrite(LED_BUILTIN, LOW);

  // Begginning the webSocket client
  wsClient.beginSSL(WS_HOST, WS_PORT, WS_URL, "","wss");
  wsClient.onEvent(onWSEvent); // Discover how this works in C++
}

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