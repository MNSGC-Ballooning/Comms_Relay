#include "GPSmanager.h"
#include "Relay.h"
#include "SDLogger.h"

//pin definitions
#define gpsSer Serial1
#define xBeeSer Serial2
#define downlinkSer Serial3
#define chipSelect 15
#define bypassButton 23

//Beacon rate - time in seconds between automatic gps beacons
#define beaconTime 30

//flag for tracking bypass switch
bool bypassFlag = false;

//Variable declarations
DataQueue gpsLogQ, radioLogQ, transmitQ;
DateTime now = DateTime(0,0,0,0,0,0);

//System setup
GPSmanager gpsManager = GPSmanager(&gpsSer, &now, &bypassFlag, beaconTime, &gpsLogQ, &transmitQ);
Relay relay = Relay(&downlinkSer, &xBeeSer, &now, &radioLogQ, &transmitQ);
SDLogger sdLogger = SDLogger(chipSelect, &now, &gpsLogQ, &radioLogQ);
System* systems[] = {&relay,
                      &gpsManager,
                      &sdLogger
                      };

//ISR to register bypass switch
void bypass_ISR() {bypassFlag = true;}

void setup() {
  //Startup notifications
  RawData* startupLog = new RawData("Comms System Start");
  RawData* startupMsg = new RawData(*startupLog);
  radioLogQ.push(startupLog);
  transmitQ.push(startupMsg);
  
  //setup bypass
  pinMode(bypassButton,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(bypassButton),bypass_ISR,FALLING);
  
  //initialize systems
  for (System* sys : systems) {
    sys->initialize();
  }
  //Because SDLogger has no ability to request a transmission, handle SD card status message here
  if (!sdLogger.isEnabled()) {
    startupMsg = new RawData("SD card Error");
    transmitQ.push(startupMsg);
  }
}

void loop() {
  //run each system
  for (System* sys : systems) {
    sys->run();
  }
}
