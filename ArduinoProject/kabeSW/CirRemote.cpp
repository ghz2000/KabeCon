#include <Arduino.h>
#include "CirRemote.h"

//- IR Trancever ------------------------------------------------------------------------

CirRemote::CirRemote(int irPort):
  irPort(irPort)
{
  digitalWrite(irPort, LOW);
  pinMode(irPort, OUTPUT);
}


void CirRemote::char2bin(uint8_t code[], char data[], int len){
  int tmp = 0;

  for(int i=0; i<len/4.0; i++){
    if(data[i] < 'A') tmp = data[i] - '0';
    else if(data[i] < 'a') tmp = data[i] - 'A' + 10;
    else tmp = data[i] - 'a' + 10;
    
    if(i%2){
      code[i/2] <<= 4;
      code[i/2] |= tmp;
    }else{ code[i/2] = tmp; }
   }
  
}


void CirRemote::irOn(long us){
  for(int i=0; i< us; i+=irPalse.career*2){
    digitalWrite(irPort, HIGH);
    delayMicroseconds(irPalse.career);
    digitalWrite(irPort, LOW);
    delayMicroseconds(irPalse.career);
  }
}

void CirRemote::irOff(long us){
    delayMicroseconds(us);
}

void CirRemote::irCode(byte data){

  for(int i=0; i<8; i++){
    if(data & 0x80){
      irOn(irPalse.data1H);
      irOff(irPalse.data1L);
    }else{
      irOn(irPalse.data0H);
      irOff(irPalse.data0L);
    }
    data <<= 1;
  }
}

void CirRemote::irOut(){
//  int freq = 38;
//  int us = 1000 / freq /2;

  //header
  irOn(7500);
  irOff(4300);

  irCode(0x01);
  irCode(0x76);
  irCode(0x20);
  irCode(0xDF);

  irOn(irPalse.readerH);
  irOff(irPalse.readerL);
}

void CirRemote::sendIR(uint8_t code[], int len){
   for(int i=0; i<len; i++){
    if(code[i/8] & 0x80){
      irOn(irPalse.data1H);
      irOff(irPalse.data1L);
    }else{
      irOn(irPalse.data0H);
      irOff(irPalse.data0L);
    }
    code[i/8] <<= 1;
  }
}

// Char Data to Bin
void CirRemote::irOut(char data[], int len){
  uint8_t code[10] = {0,0,0,0,0,0,0,0,0,0};

  char2bin(code, data, len);

  //reader
  irOn(irPalse.readerH);
  irOff(irPalse.readerL);

  sendIR(code, len);

}
