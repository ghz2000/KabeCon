#include <Arduino.h>
#include <FS.h>
#include "AdvancedOTA.h"

ESP8266WiFiMulti *m_WiFiMulti;
ESP8266WebServer *m_server;
ESP8266HTTPUpdateServer httpUpdater;

void handleSearchSSID(){
  String message = "<HTML><HEAD></HEAD>";
  message += "<BODY>";

  message += "<FORM method='GET' action='setssid'>";
  message += "<TABLE>";

  int n = WiFi.scanNetworks();
  for(int i = 0; i<n ; ++i){
    message += "<TR><TD>";
    message += "<INPUT TYPE='radio' name='SSID' value='";
    message += WiFi.SSID(i);
    message += "'></TD><TD>";
    message += i;
    message += ":";
    message += WiFi.SSID(i);
    message += " (";
    message += WiFi.RSSI(i);
    message += ")";
    message += (WiFi.encryptionType(i) == ENC_TYPE_NONE)? " ": "*";
    message += "</TD></TR>";
    delay(10);
  }
  message += "</TABLE>";

  message += "SSID Password<BR>";
  message += "<INPUT type='text' name='PASS' size='30'><BR><BR>";
  message += "<INPUT type='submit' value='set'>";
  message += "</FORM><BR><BR>";
  
  message += "</BODY></HTML>";
  m_server->send(200, "text/html", message);
}


void handleSetSSID(){
  String message = "<HTML><HEAD></HEAD>";
  message += "<BODY>";

  String ssid = m_server->arg("SSID");
  String pass = m_server->arg("PASS");

  char cssid[30];
  ssid.toCharArray(cssid, ssid.length()+1);
  char cpass[30];
  pass.toCharArray(cpass, pass.length()+1);
  m_WiFiMulti->addAP(cssid, cpass);

  Serial.print(cssid);
  Serial.print(":");
  Serial.println(cpass);

    for(int i=0; i<10; i++){
    switch(m_WiFiMulti->run()){
      case WL_CONNECTED:
        Serial.println("Connection Successful");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        break;
      case WL_NO_SSID_AVAIL:
        Serial.println("Searching SSID");
        delay(1000);
        continue;
      case WL_CONNECT_FAILED:
        Serial.println("Connection Failed. wrong password");
        delay(1000);
        continue;
      case WL_IDLE_STATUS:
        Serial.println("Connection Idle Status");
        delay(1000);
        continue;
      case WL_DISCONNECTED:
        Serial.println("Connection Disconnected");
        delay(1000);
        continue;
      default :
        Serial.println("default error");
        delay(1000);
        ESP.restart();
        continue;
    }
    break;
  }

    for(int i=0; i<10; i++){
    switch(m_WiFiMulti->run()){
      case WL_CONNECTED:
        message += "Connection Successful<BR>";
        message += "IP address: ";
        message += WiFi.localIP();
        message += "<BR>";
        break;
      case WL_NO_SSID_AVAIL:
        message += "Searching SSID<BR>";
        delay(5000);
        continue;
      case WL_CONNECT_FAILED:
        message += "Connection Failed. wrong password<BR>";
        delay(5000);
        continue;
      case WL_IDLE_STATUS:
        message += "Connection Idle Status";
        delay(5000);
        continue;
      case WL_DISCONNECTED:
        message += "Connection Disconnected";
        delay(5000);
        continue;
      default :
        message += "default error";
        delay(5000);
        ESP.restart();
        continue;
    }
    break;
  }

  message += "</BODY></HTML>";
  m_server->send(200, "text/html", message);
}



void wifiSetup(ESP8266WiFiMulti *WiFiMulti, ESP8266WebServer *server){
  m_WiFiMulti = WiFiMulti;
  m_server = server;

//  Serial.begin(74880);
  Serial.println("Booting");


  ////// WiFi Connection

  WiFi.mode(WIFI_STA);

  WiFi.softAP(ap_ssid, ap_pass);
  Serial.print("AP mode IP:");
  Serial.println(WiFi.softAPIP());


  WiFiMulti->addAP(sta_ssid, sta_pass);


  ////// OTA Function
  
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

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
  ArduinoOTA.begin();


  ////// WebServer
  if (MDNS.begin(mdns_name)) {
    Serial.println("MDNS responder started");
  }
  
  m_server->on("/ssid", handleSearchSSID);
  m_server->on("/setssid", handleSetSSID);


  m_server->onNotFound([](){
      m_server->send(404, "text/plain", "FileNotFound");
  });

  httpUpdater.setup(m_server);

  m_server->begin();
  Serial.println("HTTP server started");
  
}

void listener(){
  ArduinoOTA.handle();
  m_server->handleClient();
}

 
