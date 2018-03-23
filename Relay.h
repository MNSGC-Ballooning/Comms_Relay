#ifndef Relay_h
#define Relay_h

#include "System.h"

//class to drive serial-based radio devices (XBee, RFD, etc.)
class Radio {
  public:
    Radio(HardwareSerial* port);
    virtual ~Radio() {}
    void begin(long baud);
    void print(String message);
    String read();
  private:
    HardwareSerial* port; //the hardware port the radio is connected to
};

//class to handle overall relay system
class Relay: public System {
  public:
    Relay(HardwareSerial* downlinkSerial, HardwareSerial* xBeeSerial, DateTime* now, DataQueue* logQ, DataQueue* transmitQ);
    void initialize();
    void run();
  private:
    Radio groundRadio, xBee; //radio objects for xbee and ground station connections
    DataQueue* logQ,* transmitQ; //outgoing queue for logging messages, and incoming queue of data to be transmitted
};

#endif
