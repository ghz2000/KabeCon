extern "C" {
  #include <user_interface.h>
}

#include "AdvancedOTA.h"
#include <Servo.h>

ESP8266WiFiMulti WiFiMulti;
ESP8266WebServer server(80);

Servo myservo;

#define servoPow 13
//#define servoSig 1  //TX
#define servoSig 14  //REMOTE
#define leftSW 4
#define rightSW 5
#define bootSW 0

int pos = 100;
int minimum = 256;
int maximum = 0;

bool isOn = false;
bool amazonFlag = false;

void ICACHE_FLASH_ATTR wifi_handle_event_cb(System_Event_t *evt) {

  if (evt->event == EVENT_SOFTAPMODE_PROBEREQRECVED){   //プローブリクエスト以外は無視
    uint8_t* mac = evt->event_info.ap_probereqrecved.mac;
    if ( checkMAC(mac) ){
        amazonFlag = true;
//      Serial.print("Probe MAC: ");
//      Serial.println( getStrMAC(mac) );
    }
  }
  
  if (evt->event != EVENT_SOFTAPMODE_STACONNECTED) return; 

  uint8_t* mac = evt->event_info.sta_connected.mac;
//  Serial.print("MAC: ");
//  Serial.println( getStrMAC(mac) );
}

String getStrMAC(uint8_t mac[6]){
  String res = String(mac[0], HEX) + ":" + String(mac[1], HEX) + ":" + String(mac[2], HEX) + ":" +
                String(mac[3], HEX) + ":" + String(mac[4], HEX) + ":" + String(mac[5], HEX);
  return res;
}

bool checkMAC(uint8_t* mac){
  String macStr = getStrMAC(mac) ;

  if( macStr.compareTo("ab:c:d:ef:10:23") ==0 ){
    return 1;
  }

  return 0;
}

void setup() {
//  Serial.begin(74880);
  Serial.end();
  
  digitalWrite(servoPow, LOW);
  pinMode(servoPow, OUTPUT);
  pinMode(leftSW, INPUT);
  pinMode(rightSW, INPUT);
  pinMode(bootSW, INPUT);
  myservo.attach(servoSig);
  setServoParam(false);
  
  wifiSetup(&WiFiMulti, &server);
  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.on("/turn", handleTurn);

}




void loop() {
  static unsigned long previousAmazon = 0;
  static unsigned long currentMillis = 0;
  const unsigned long intervalAmazon = 10000;
  currentMillis = millis();
  listener();

  if (digitalRead(leftSW) == LOW) {
    swOff();
  }

  if (digitalRead(rightSW) == LOW) {
    swOn();
  }

  if (digitalRead(bootSW) == LOW) {
    setServoParam(true);
    while (digitalRead(bootSW) == LOW) {}
  }

  if (amazonFlag){
    if(currentMillis - previousAmazon >= intervalAmazon){
      swTurn();
      previousAmazon = currentMillis;
    }
    amazonFlag = false;
  }

  delay(100);
}

void setServoParam(bool wifiSetup) {
  const unsigned long interval = 1000;
  unsigned long previousMills = 0;
  unsigned long currentMills = millis();

  const int sensitive = 100;

  minimum = 256;
  maximum = 0;
  pos = 100;
//  int minimum = 256;
//  int maximum = 0;
  myservo.write(pos);

  while (digitalRead(bootSW) == LOW) {}

  while(1){
    if(wifiSetup)listener();
    digitalWrite(servoPow, HIGH);

    currentMills = millis();
    if(currentMills - previousMills >= interval){
      previousMills = currentMills;
      digitalWrite(leftSW, LOW);
      pinMode(leftSW, OUTPUT);
      delay(100);
      digitalWrite(leftSW,HIGH);
      pinMode(leftSW, INPUT);
    }

    if (digitalRead(leftSW) == LOW) {
      if(0 < pos) pos -= 1;
      if(pos < minimum) minimum = pos;
      delay(sensitive);
    }

    if (digitalRead(rightSW) == LOW) {
      if(pos < 256) pos += 1;
    if(maximum < pos) maximum = pos;
      delay(sensitive);
    }

    myservo.write(pos);

    if (digitalRead(bootSW) == LOW) {
      digitalWrite(servoPow, LOW);
      break;
    }
  }
}

void swOn() {
  isOn = true;
  pos = maximum;
  digitalWrite(servoPow, HIGH); //サーボを動かすときは電源を入れます
  myservo.write(pos);
  delay(1000);
  myservo.write(100);
  delay(500);
  digitalWrite(servoPow, LOW);  //動かし終わったら電源を切ります
}

void swOff() {
  isOn = false;
  pos = minimum;
  digitalWrite(servoPow, HIGH);
  myservo.write(pos);
  delay(1000);
  myservo.write(100);
  delay(500);
  digitalWrite(servoPow, LOW);
}

void swTurn(){
  if(isOn){
    swOff();
  }else{
    swOn();
  }
}

void handleRoot() {
  String tempServo = server.arg("servo");
  if (tempServo.toInt() != 0) {
    pos = tempServo.toInt();
    myservo.write(pos);  //電源入れてないので動きません｡
  }

  String message = "<HTML><HEAD>";
  message += "<style type='text/css'>";
  message += ".square_btn{";
  message += "    display: inline-block;";
  message += "    padding: 0.5em 1em;";
  message += "    text-decoration: none;";
  message += "    background: #668ad8;";
  message += "    color: #FFF;";
  message += "    border-bottom: solid 4px #627295;";
  message += "    border-radius: 3px;";
  message += "}";
  message += ".square_btn:active {";
  message += "    -ms-transform: translateY(4px);";
  message += "    -webkit-transform: translateY(4px);";
  message += "    transform: translateY(4px);";
  message += "    border-bottom: none;";
  message += "}";
  message += "html{font-size: 62.5%;}";
  message += "body{font-size: 12em;}";
  message += "</style></HEAD>";
  message += "<BODY>";

  message += "<a href='/on' class='square_btn'>ON</a>";
  message += "<a href='/off' class='square_btn'>OFF</a><BR>";
 message += "<a href='/turn' class='square_btn'>Change</a><BR><BR>";

  message += "now value=";
  message += pos;
  message += "<BR><HR>";
 
  message += "maximum =";
  message += maximum;
  message += "<BR>";
  message += "minimum =";
  message += minimum;
  message += "<BR><BR><HR>";

  message += "<FORM method='GET' action='/'>";
  message += "<TABLE>";
  message += "<TR><TD>";
  message += "<INPUT TYPE='value' name='servo' value=''";
  message += "'></TD><TD>";
  message += "</TD></TR>";
  message += "</TABLE>";

  message += "<INPUT type='submit' value='set'>";
  message += "</FORM><BR><BR>";
  
  message += "</BODY></HTML>";

  server.send(200, "text/html", message);
}

void handleOn() {
  pos = maximum;
  handleRoot();
  swOn();
}
void handleOff() {
  pos = minimum;
  handleRoot();
  swOff();
}


void handleTurn(){
 if(isOn){
    pos = minimum;
  }else{
    pos = maximum;
  }
  handleRoot();
  swTurn();
}

