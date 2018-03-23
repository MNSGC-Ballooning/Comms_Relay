#ifndef GPSmanager_h
#define GPSmanager_h

#include <UbloxGPS.h>
#include <RepeatingAction.h>

#include "System.h"

//Class to handle GPS module and gather data for transmission and logging
class GPSmanager: public System {
  public:
    GPSmanager(HardwareSerial* port, DateTime* now, bool* bypassFlag, byte beaconTime, DataQueue* logQ, DataQueue* transmitQ);
    void initialize();
    void run();
  private:
    HardwareSerial* port; //the hardware port the gps is connected to
    byte beaconTime; //time in seconds between automatic GPS beacons
    FlightGPS* gps; //actual gps driver object
    DataQueue* logQ,* transmitQ; //Queues for gps data to be logged and transmitted
    //class to handle automatic logging and beaconing actions
    class GPSlogAction: public RepeatingAction {
      public:
        GPSlogAction(byte beaconTime, FlightGPS* gps, DataQueue* logQ, DataQueue* transmitQ);
      private:
        byte beaconTime;  //time in seconds between automatic GPS beacons
        byte reps; //tracks log cycles (each about 1 second) and triggers beaconing when needed
        FlightGPS* gps = NULL; //also needs access to gps driver
        DataQueue* logQ,* transmitQ; //likewise for queue objects
        void execute();
    };
    GPSlogAction* gpsLogger;
};

#endif
