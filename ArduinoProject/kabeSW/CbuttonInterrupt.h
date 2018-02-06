#include <Arduino.h>
#include <Ticker.h>


#ifndef __CbuttonInterrupt__
#define __CbuttonInterrupt__

#define DEBOUNDS 40
#define DOUBLECLICK 300
#define LONGPRESS 300


class CbuttonInterrupt
{
private:
  
  Ticker btnTicker;

  volatile int m_pin;

  volatile unsigned long m_downMillis;
  volatile int m_count;
  volatile bool m_isDown;
  volatile bool m_isLongPress;

  void (*p_intChanged)();
  void (*p_intFunction)();
  void (*p_intDeBounced)();

public:
//  void initialize(int pin);
  void initialize(int pin, void (*intChanged)(), void (*intDeBounced)(), void (*intFunction)());

  void changed();
  void deBounced();
  void function();

  void setInterrupt();

  void clear();
  int getClick();
  int getLong();

};


#endif __CbuttonInterrupt__

