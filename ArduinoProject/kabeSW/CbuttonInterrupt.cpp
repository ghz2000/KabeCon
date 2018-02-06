#include <Arduino.h>
#include "CbuttonInterrupt.h"

#include <Ticker.h>


void CbuttonInterrupt::initialize(int pin, void (*intChanged)(), void (*intDeBounced)(), void (*intFunction)()){
  m_pin = pin;
  p_intFunction = intFunction;
  p_intChanged = intChanged;
  p_intDeBounced = intDeBounced;
  
  m_downMillis = 0;
  m_count = 0;
  m_isDown = false;
  m_isLongPress = false;
}

void CbuttonInterrupt::setInterrupt(){
  attachInterrupt(m_pin, p_intChanged, CHANGE);
}

void CbuttonInterrupt::changed(){
  detachInterrupt(m_pin);
  btnTicker.detach();
  btnTicker.once_ms(DEBOUNDS, p_intDeBounced);
}

void CbuttonInterrupt::deBounced(){
  if( digitalRead(m_pin) ){
    //HIGH

    if( LONGPRESS < millis() - m_downMillis ){
      //longPress
      m_isDown = false;
      m_isLongPress = true;
      btnTicker.once_ms(DOUBLECLICK, p_intFunction);
      attachInterrupt(m_pin, p_intChanged, CHANGE); 
    }else{
      //press
      //ダブルクリックにならないかチェックする
      m_isDown = false;
      btnTicker.once_ms(DOUBLECLICK, p_intFunction);
      attachInterrupt(m_pin, p_intChanged, CHANGE); 
    }
  }else{
    //LOW
    if(!m_isDown){
      m_downMillis = millis();
      m_count++;
    }
      attachInterrupt(m_pin, p_intChanged, CHANGE); 
  }
}

void CbuttonInterrupt::function(){
  //ダブルクリック判定にならなかったのでクリックを実行する
  
  if(m_isLongPress) Serial.println("Long Press");
  Serial.print(m_count);
  Serial.println(" Times");

  //clear();

}

void CbuttonInterrupt::clear(){
  m_downMillis = 0;
  m_count = 0;
  m_isDown = false;
  m_isLongPress = false;
}

int CbuttonInterrupt::getClick(){
  return m_count;
}

int CbuttonInterrupt::getLong(){
  return m_isLongPress;
}

