#include "GPSmanager.h"

//GPSmanager constructor
GPSmanager::GPSmanager(HardwareSerial* port, DateTime* now, bool* bypassFlag, byte beaconTime, DataQueue* logQ, DataQueue* transmitQ):
  System(now,bypassFlag), port(port), beaconTime(beaconTime), logQ(logQ), transmitQ(transmitQ) {
}

//setup() code for GPSmanager
void GPSmanager::initialize() {
  port->begin(9600);

  //check for 'mu' 'B' prefix that indicates Ublox is present. If so, create Ublox gps object
  for (unsigned long startTime = millis(); millis() - startTime < 10000;) {
    if (port->read() == 0xB5) {
      delay(1);
      if (port->read() == 0x62) {
        gps = new UbloxGPS(port);
        break;
      }
    }
  }
  if (gps == NULL) gps = new FlightGPS(port); //if no Ublox detected, use default gps object
  gps->initialize(); //gps object initialization code
  gpsLogger = new GPSlogAction(beaconTime, gps, logQ, transmitQ); //create gpsLogger object

  //Loop until lock is acquired. If this does not happen, bypass switch can be used to disable gps system and continue setup
  while (gps->getFixAge() == 0xFFFFFFFF) {
    gps->update();
    if (*bypassFlag) {
      enabled = false; //currently this doesn't have any effect. Might implement some altered behavior in future
      return; //primary purpose of bypass is to break out of infinite loop. Keep GPS running in case lock is eventually acquired
    }
  }
  //define global time object for all systems
  *now = DateTime(gps->getYear(), gps->getMonth(), gps->getDay(), gps->getHour(), gps->getMinute(), gps->getSecond());
}

//loop() code for GPSmanager
void GPSmanager::run() {
  gps->update(); //get most recent gps data
  //update global time object
  *now = DateTime(gps->getYear(), gps->getMonth(), gps->getDay(), gps->getHour(), gps->getMinute(), gps->getSecond());
  //check for necessary log or beacon actions
  gpsLogger->check();
}

//GPSlogAction constructor
GPSmanager::GPSlogAction::GPSlogAction(byte beaconTime, FlightGPS* gps, DataQueue* logQ, DataQueue* transmitQ):
  RepeatingAction(1000), beaconTime(beaconTime), gps(gps), logQ(logQ), transmitQ(transmitQ) {
}

//Function to handle gps logging and beaconing; called automatically at defined interval
void GPSmanager::GPSlogAction::execute() {
  //create GPS data object for logging and transmiting
  GPSdata* logData = new GPSdata(gps->getHour(), gps->getMinute(), gps->getSecond(),
                                  gps->getLat(), gps->getLon(), gps->getAlt(), gps->getSats());
  //always push data to log queue
  logQ->push(logData);
  reps++;
  if (reps == beaconTime) { //time for gps data transmission
    //need separate but identical data object for transmit queue
    GPSdata* beaconData = new GPSdata(*logData);
    transmitQ->push(beaconData);
    reps = 0;
  }
}

