#include "Relay.h"

Radio::Radio(HardwareSerial* port):
  port(port) {
}

void Radio::begin(long baud) {
  port->begin(baud);
  port->setTimeout(10);
}

void Radio::print(String message) {
  port->print(message);
}

String Radio::read() {
  return port->readStringUntil('!');
}

Relay::Relay(HardwareSerial* downlinkSerial, HardwareSerial* xBeeSerial, DateTime* now, DataQueue* logQ, DataQueue* transmitQ):
  System(now), groundRadio(downlinkSerial), xBee(xBeeSerial), logQ(logQ), transmitQ(transmitQ) {
}

void Relay::initialize() {
  groundRadio.begin(9600);
  xBee.begin(9600);
}

void Relay::run() {
  {
    char data[42] = {0};
    Data* gpsData = transmitQ->pop();
    gpsData->toCharArray(data);
    groundRadio.print(String(data) + '!');
    delete gpsData; gpsData = NULL;
  }
  {
    String downlink = xBee.read();
    if (!downlink.equals("")) {
      logQ->push(new RadioData('X', downlink));
      groundRadio.print(downlink);
    }
  }
  {
    String uplink = groundRadio.read();
    if (!uplink.equals("")) {
      logQ->push(new RadioData('G', uplink));
      if (uplink.substring(0,5).equals("IMAGE"))
        groundRadio.print("Error 404: Pi not found!");
      else
        xBee.print(uplink);
    }
  }
}

