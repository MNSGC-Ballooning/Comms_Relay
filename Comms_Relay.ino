#include "GPSmanager.h"
#include "Relay.h"
#include "SDLogger.h"

#define gpsSerial Serial1
#define xBeeSerial Serial2
#define downlinkSerial Serial3
#define chipSelect 14

DataQueue gpsLogQ, radioLogQ, transmitQ;

DateTime now = DateTime(0,0,0,0,0,0);

System* systems[] = {new GPSmanager(&gpsSerial, &now, &gpsLogQ, &transmitQ),
                      new Relay(&downlinkSerial, &xBeeSerial, &now, &radioLogQ, &transmitQ),
                      new SDLogger(chipSelect, &now, &gpsLogQ, &radioLogQ)};

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
