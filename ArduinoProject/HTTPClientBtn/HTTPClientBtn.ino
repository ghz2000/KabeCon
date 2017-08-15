#include "AdvancedOTA.h"
#include <WiFiUdp.h>
#include <ESP8266HTTPClient.h>

ESP8266WiFiMulti WiFiMulti;
ESP8266WebServer server(80);

HTTPClient http;

#define Btn 4

void setup() {
  Serial.begin(115200);
  wifiSetup(&WiFiMulti, &server);

 digitalWrite(Btn, HIGH);
 pinMode(Btn, INPUT); 

}



void loop() {
  listener();
 
  if( digitalRead(Btn) == LOW){
    Serial.println("Btn Pushed");
    delay(100);
    
        HTTPClient http;

        Serial.print("[HTTP] begin...\n");
        // configure traged server and url
        http.begin("http://192.168.4.1/turn"); //HTTP

        Serial.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                Serial.println(payload);
            }
        } else {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    
    
    while( digitalRead(Btn) == LOW){}
  }

}



