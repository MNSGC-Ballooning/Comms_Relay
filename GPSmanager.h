#ifndef GPSmanager_h
#define GPSmanager_h

#include <UbloxGPS.h>
#include <RepeatingAction.h>

#include "System.h"

class GPSmanager: public System {
  public:
    GPSmanager(HardwareSerial* port, DateTime* now, bool* bypassFlag, DataQueue* logQ, DataQueue* transmitQ);
    void initialize();
    void run();
  private:
    HardwareSerial* port;
    FlightGPS* gps;
    DataQueue* logQ,* transmitQ;
    class GPSlogAction: public RepeatingAction {
      public:
        GPSlogAction(byte beaconTime, FlightGPS* gps, DataQueue* logQ, DataQueue* transmitQ);
      private:
        byte beaconTime;  //time in seconds between automatic GPS beacons
        byte reps;
        FlightGPS* gps;
        DataQueue* logQ,* transmitQ;
        void execute();
    };
    GPSlogAction* gpsLogger;
};

#endif
