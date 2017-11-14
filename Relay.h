#ifndef Relay_h
#define Relay_h

#include "System.h"

class Radio {
  public:
    Radio(HardwareSerial* port);
    virtual ~Radio() {}
    void begin(long baud);
    void print(String message);
    String read();
  private:
    HardwareSerial* port;
};

class Relay: public System {
  public:
    Relay(HardwareSerial* downlinkSerial, HardwareSerial* xBeeSerial, DateTime* now, DataQueue* logQ, DataQueue* transmitQ);
    void initialize();
    void run();
  private:
    Radio groundRadio, xBee;
    DataQueue* logQ,* transmitQ;
};

#endif
