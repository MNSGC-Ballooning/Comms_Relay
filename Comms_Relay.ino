#include "GPSmanager.h"
#include "Relay.h"
#include "SDLogger.h"

#define gpsSer Serial1
#define xBeeSer Serial2
#define downlinkSer Serial3
#define chipSelect 14

DataQueue gpsLogQ, radioLogQ, transmitQ;

DateTime now = DateTime(0,0,0,0,0,0);
GPSmanager gpsManager = GPSmanager(&gpsSer, &now, &gpsLogQ, &transmitQ);
//Relay relay = Relay(&downlinkSer, &xBeeSer, &now, &radioLogQ, &transmitQ);
SDLogger sdLogger = SDLogger(chipSelect, &now, &gpsLogQ, &radioLogQ);
System* systems[] = {//&gpsManager,
                      //&relay,
                      &sdLogger};

void setup() {
  for (System* sys : systems) {
    sys->initialize();
  }
}

void loop() {
  for (System* sys : systems) {
    sys->run();
  }
}
