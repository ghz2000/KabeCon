#include <Arduino.h>
#include <FS.h>
#include "AdvancedOTA.h"
#include "CiniParser.h"

ESP8266WiFiMulti *m_WiFiMulti;
ESP8266WebServer *m_server;
ESP8266HTTPUpdateServer httpUpdater;
#define INIFNM "/config.ini"

void handleSearchSSID(){
  CiniParser testini;
  if(testini.setIniFileName( INIFNM )){
    Serial.println("File not exist");
  }
  String valu;
  
  String message = "<HTML><HEAD></HEAD>";
  message += "<BODY>";

//Wi-Fi 設定表示
  message += "<B>WiFi Setting</B><BR>";
  testini.rwIni("WiFi", "SSID", &valu, READ);
  message += "SSID : ";
  message += valu;
  testini.rwIni("WiFi", "PASS", &valu, READ);
  message += "<BR>PASS : ";
  message += valu;
  message += "<BR>";
  message += "WiFi IP Address : ";
  message += String(WiFi.localIP()[0]) + '.' + String(WiFi.localIP()[1]) + '.' + String(WiFi.localIP()[2]) + '.' + String(WiFi.localIP()[3]);
  message += "<BR>";
  message += "AP mode Address : ";
  message += String(WiFi.softAPIP()[0]) + '.' + String(WiFi.softAPIP()[1]) + '.' + String(WiFi.softAPIP()[2]) + '.' + String(WiFi.softAPIP()[3]);
  message += "<BR>";

  message += "Wi-Fi Status :";
  switch(m_WiFiMulti->run()){
  case WL_CONNECTED:
    message += "CONNECTED";
    break;
  case WL_NO_SSID_AVAIL:
    message += "NO SSID AVAIL";
    break;
  case WL_CONNECT_FAILED:
    message += "CONNECT FAILED";
    break;
  case WL_IDLE_STATUS:
    message += "IDLE";
    break;
  case WL_DISCONNECTED:
    message += "DISCONNECTED";
    break;
  default :
    message += "ERROR";
  }
  message += "<BR><BR>";

  message += "<BR><BR><B>Serched SSID</B>";
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
  CiniParser testini;
  if(testini.setIniFileName( INIFNM )){
//    Serial.println("File not exist");
    testini.createIniFile();
  }

  String message = "<HTML><HEAD></HEAD>";
  message += "<BODY>";

  String ssid = m_server->arg("SSID");
  String pass = m_server->arg("PASS");

  testini.rwIni("WiFi", "SSID", &ssid, WRITE);
  testini.rwIni("WiFi", "PASS", &pass, WRITE);

  char cssid[30];
  ssid.toCharArray(cssid, ssid.length()+1);
  char cpass[30];
  pass.toCharArray(cpass, pass.length()+1);
  m_WiFiMulti->addAP(cssid, cpass);

  Serial.print(cssid);
  Serial.print(":");
  Serial.println(cpass);

  String ipadd;

  for(int i=0; i<10; i++){
    switch(m_WiFiMulti->run()){
      case WL_CONNECTED:
        Serial.println("Connection Successful");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        message += "Connection Successful<BR>";
        message += "WiFi IP Address : ";
        ipadd = String(WiFi.localIP()[0]) + '.' + String(WiFi.localIP()[1]) + '.' + String(WiFi.localIP()[2]) + '.' + String(WiFi.localIP()[3]);
        message += ipadd;
        message += "<BR><BR><BR>";
        message += "<A HREF=http://" + ipadd + "> http://" + ipadd + "</A><BR>";
        message += "<A HREF=http://192.168.4.1> http://192.168.4.1</A><BR>";
        break;
      case WL_NO_SSID_AVAIL:
        Serial.println("Searching SSID");
        message += "Searching SSID<BR>";
        delay(1000);
        continue;
      case WL_CONNECT_FAILED:
        Serial.println("Connection Failed. wrong password");
        message += "Connection Failed. wrong password<BR>";
        delay(1000);
        continue;
      case WL_IDLE_STATUS:
        Serial.println("Connection Idle Status");
        message += "Connection Idle Status";
        delay(1000);
        continue;
      case WL_DISCONNECTED:
        Serial.println("Connection Disconnected");
        message += "Connection Disconnected";
        delay(1000);
        continue;
      default :
        Serial.println("default error");
        message += "default error";
        delay(1000);
        ESP.restart();
        continue;
    }
    break;
  }

  message += "</BODY></HTML>";
  m_server->send(200, "text/html", message);
}

//-------------------------------------------------------------------------


String getContentType(String filename){
  if(m_server->hasArg("download")) return "application/octet-stream";
  else if(filename.endsWith(".ini")) return "text/plain";
  else if(filename.endsWith(".txt")) return "text/plain";
  else if(filename.endsWith(".htm")) return "text/html";
  else if(filename.endsWith(".html")) return "text/html";
  else if(filename.endsWith(".css")) return "text/css";
  else if(filename.endsWith(".js")) return "application/javascript";
  else if(filename.endsWith(".png")) return "image/png";
  else if(filename.endsWith(".gif")) return "image/gif";
  else if(filename.endsWith(".jpg")) return "image/jpeg";
  else if(filename.endsWith(".ico")) return "image/x-icon";
  else if(filename.endsWith(".xml")) return "text/xml";
  else if(filename.endsWith(".pdf")) return "application/x-pdf";
  else if(filename.endsWith(".zip")) return "application/x-zip";
  else if(filename.endsWith(".gz")) return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path){
  Serial.println("handleFileRead: " + path);
  if(path.endsWith("/")) path += "index.htm";
  String contentType = getContentType(path);
  String pathWithGz = path + ".gz";
  if(SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)){
    if(SPIFFS.exists(pathWithGz))
      path += ".gz";
    File file = SPIFFS.open(path, "r");
    size_t sent = m_server->streamFile(file, contentType);
    file.close();
    return true;
  }
  return false;
}

//-------------------------------------------------------------------------

void wifiSetup(ESP8266WiFiMulti *WiFiMulti, ESP8266WebServer *server){
  m_WiFiMulti = WiFiMulti;
  m_server = server;

//  Serial.begin(74880);
  Serial.println("Booting");


  ////// WiFi Connection
  CiniParser ini;
  String ssid;
  String pass;
  if(ini.setIniFileName( INIFNM )){
    Serial.println("File not exist");
  }

  WiFi.mode(WIFI_AP_STA);

  ini.rwIni("WiFi", "APSSID", &ssid, READ);
  ini.rwIni("WiFi", "APPASS", &pass, READ);  
  if(ssid.length()*pass.length()){
    WiFi.softAP(ssid.c_str(), pass.c_str());
  }else{
    WiFi.softAP(ap_ssid, ap_pass);
  }
  Serial.print("AP mode IP:");
  Serial.println(WiFi.softAPIP());
  Serial.print(ap_ssid);
  Serial.print(" : ");
  Serial.println(ap_pass);


  ini.rwIni("WiFi", "SSID", &ssid, READ);
  ini.rwIni("WiFi", "PASS", &pass, READ);
  if(ssid.length()*pass.length()){
    WiFiMulti->addAP(ssid.c_str(), pass.c_str());
    
    Serial.print(ssid);
    Serial.print(":");
    Serial.println(pass);
    
    for(int i=0; i<10; i++){
      switch(WiFiMulti->run()){
        case WL_CONNECTED:
          Serial.println("Connection Successful");
          Serial.print("IP address: ");
          Serial.println(WiFi.localIP());
          break;
        case WL_NO_SSID_AVAIL:
          Serial.println("Searching SSID");
          delay(5000);
          continue;
        case WL_CONNECT_FAILED:
          Serial.println("Connection Failed. wrong password");
          delay(5000);
          continue;
        case WL_IDLE_STATUS:
          Serial.println("Connection Idle Status");
          delay(5000);
          continue;
        case WL_DISCONNECTED:
          Serial.println("Connection Disconnected");
          delay(5000);
          continue;
        default :
          Serial.println("default error");
          delay(5000);
          ESP.restart();
          continue;
      }
      break;
    }
  }



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
    if(!handleFileRead(m_server->uri()))
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
