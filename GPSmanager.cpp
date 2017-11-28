#include "GPSmanager.h"

GPSmanager::GPSmanager(HardwareSerial* port, DateTime* now, DataQueue* logQ, DataQueue* transmitQ):
  System(now), port(port), logQ(logQ), transmitQ(transmitQ) {
}

void GPSmanager::initialize() {
  port->begin(9600);
  for (unsigned long startTime = millis(); millis() - startTime < 10000;) {
    if (port->read() == 0xB5) {
      delay(1);
      if (port->read() == 0x62) {
        gps = new UbloxGPS(port);
        break;
      }
    }
  }
  if (gps == NULL) gps = new FlightGPS(port);
  gps->initialize();
  gps->update();
  *now = DateTime(gps->getYear(), gps->getMonth(), gps->getDay(), gps->getHour(), gps->getMinute(), gps->getSecond());
  gpsLogger = new GPSlogAction(3, gps, logQ, transmitQ);
}

void GPSmanager::run() {
  gps->update();
  *now = DateTime(gps->getYear(), gps->getMonth(), gps->getDay(), gps->getHour(), gps->getMinute(), gps->getSecond());
  gpsLogger->check();
}

GPSmanager::GPSlogAction::GPSlogAction(byte beaconTime, FlightGPS* gps, DataQueue* logQ, DataQueue* transmitQ):
  RepeatingAction(1000), beaconTime(beaconTime), gps(gps), logQ(logQ), transmitQ(transmitQ) {
}

void GPSmanager::GPSlogAction::execute() {
  GPSdata* logData = new GPSdata(gps->getHour(), gps->getMinute(), gps->getSecond(),
                                  gps->getLat(), gps->getLon(), gps->getAlt(), gps->getSats());
  
  logQ->push(logData);
  if (reps == 2) {
    GPSdata* beaconData = new GPSdata(*logData);
    transmitQ->push(beaconData);
    reps = 0;
  }
  else reps++;
}

