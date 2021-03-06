#include "Relay.h"

//Radio constructor
Radio::Radio(HardwareSerial* port):
  port(port) {
}

//Setup code for Radio - simply opens serial connection
void Radio::begin(long baud) {
  port->begin(baud);
  port->setTimeout(10);
}

//Wrapper for Serial.print() on Radio port
void Radio::print(String message) {
  port->print(message);
}

//All Comms Relay transmissions end in '!'; read() looks for terminating character and returns string up to that point
String Radio::read() {
  return port->readStringUntil('!');
}

//Relay constructor
Relay::Relay(HardwareSerial* downlinkSerial, HardwareSerial* xBeeSerial, DateTime* now, DataQueue* logQ, DataQueue* transmitQ):
  System(now,NULL), groundRadio(downlinkSerial), xBee(xBeeSerial), logQ(logQ), transmitQ(transmitQ) {
}

//Relay setup() code
void Relay::initialize() {
  //start radios at appropriate baud rate
  groundRadio.begin(38400);
  xBee.begin(9600);
  //send any waiting startup messages
  for (Data* startupMsg = transmitQ->pop(); startupMsg; startupMsg = transmitQ->pop()) {
    char data[42] = {0};
    startupMsg->toCharArray(data);
    groundRadio.print(String(data) + "!\n");
    delete startupMsg;
  }
}

//Relay loop() code
void Relay::run() {
  //If there's any data in the transmit queue, send it down
  for (Data* gpsData = transmitQ->pop(); gpsData; gpsData = transmitQ->pop()) {
    char data[42] = {0};
    gpsData->toCharArray(data);
    groundRadio.print(String(data) + "!\n");
    delete gpsData;
  }
  //Check for incoming XBee messages, and relay to ground
  for (String downlink = xBee.read(); !downlink.equals(""); downlink = xBee.read()) {
    logQ->push(new RadioData(now, 'X', downlink));
    if (downlink.charAt(downlink.length()-1) == '\n')
      groundRadio.print(downlink);
    else
      groundRadio.print(downlink + "!\n");
  }
  //Repeat for incoming ground station messages to xbee
  for (String uplink = groundRadio.read(); !uplink.equals(""); uplink = groundRadio.read()) {
    logQ->push(new RadioData(now, 'G', uplink));
    if (uplink.substring(0,5).equals("IMAGE")) //command meant for Raspberry-Pi based still image system
      groundRadio.print("Error 404: Pi not found!\n");
    else
      xBee.print(uplink + "!\n");
  }
}

