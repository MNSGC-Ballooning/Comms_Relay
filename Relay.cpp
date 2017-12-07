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
  System(now,NULL), groundRadio(downlinkSerial), xBee(xBeeSerial), logQ(logQ), transmitQ(transmitQ) {
}

void Relay::initialize() {
  groundRadio.begin(9600);
  xBee.begin(9600);
}

void Relay::run() {
  for (Data* gpsData = transmitQ->pop(); gpsData; gpsData = transmitQ->pop()) {
    char data[42] = {0};
    gpsData->toCharArray(data);
    groundRadio.print(String(data) + '!');
    delete gpsData;
  }
  for (String downlink = xBee.read(); !downlink.equals(""); downlink = xBee.read()) {
    logQ->push(new RadioData(now, 'X', downlink));
    groundRadio.print(downlink);
  }
  for (String uplink = groundRadio.read(); !uplink.equals(""); uplink = groundRadio.read()) {
    logQ->push(new RadioData(now, 'G', uplink));
    if (uplink.substring(0,5).equals("IMAGE"))
      groundRadio.print("Error 404: Pi not found!");
    else
      xBee.print(uplink);
  }
}

