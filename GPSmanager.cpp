#include "GPSmanager.h"

//GPSmanager constructor
GPSmanager::GPSmanager(HardwareSerial* port, DateTime* now, bool* bypassFlag, byte beaconTime, DataQueue* logQ, DataQueue* transmitQ):
  System(now,bypassFlag), port(port), beaconTime(beaconTime), logQ(logQ), transmitQ(transmitQ) {
}

//setup() code for GPSmanager
void GPSmanager::initialize() {
  port->begin(9600);
  RawData* gpsLog, * gpsMsg;
  //check for 'mu' 'B' prefix that indicates Ublox is present. If so, create Ublox gps object
  for (unsigned long startTime = millis(); millis() - startTime < 10000;) {
    if (port->read() == 0xB5) {
      delay(1);
      if (port->read() == 0x62) {
        gps = new UbloxGPS(port);
        gpsLog = new RawData("Ublox Detected");
        gpsMsg = new RawData(*gpsLog);
        logQ->push(gpsLog);
        transmitQ->push(gpsMsg);
        break;
      }
    }
  }
  if (gps == NULL) {
    gps = new FlightGPS(port); //if no Ublox detected, use default gps object
    gpsLog = new RawData("No Ublox Detected");
    gpsMsg = new RawData(*gpsLog);
    logQ->push(gpsLog);
    transmitQ->push(gpsMsg);
  }
  gps->initialize(); //gps object initialization code
  gpsLogger = new GPSlogAction(beaconTime, gps, logQ, transmitQ); //create gpsLogger object

  //Loop until lock is acquired. If this does not happen, bypass switch can be used to disable gps system and continue setup
  while (gps->getFixAge() == 0xFFFFFFFF) {
    gps->update();
    if (*bypassFlag) {
      enabled = false; //currently this doesn't have any effect. Might implement some altered behavior in future
      gpsLog = new RawData("GPS Bypassed - fix not acquired");
      gpsMsg = new RawData(*gpsLog);
      logQ->push(gpsLog);
      transmitQ->push(gpsMsg);
      return; //primary purpose of bypass is to break out of infinite loop. Keep GPS running in case lock is eventually acquired
    }
  }
  //Record lock acquisition
  gpsLog = new RawData("Fix acquired");
  gpsMsg = new RawData(*gpsLog);
  logQ->push(gpsLog);
  transmitQ->push(gpsMsg);
  
  //define global time object for all systems
  *now = DateTime(gps->getYear(), gps->getMonth(), gps->getDay(), gps->getHour(), gps->getMinute(), gps->getSecond());
}

//loop() code for GPSmanager
void GPSmanager::run() {
  gps->update(); //get most recent gps data
  if (!enabled) {
    if (gps->getFixAge() != 0xFFFFFFFF) enabled = true;//for when this becomes relevant
  }
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

