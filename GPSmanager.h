#ifndef GPSmanager_h
#define GPSmanager_h

#include <UbloxGPS.h>
#include <RepeatingAction.h>

#include "System.h"

class GPSmanager: public System {
  public:
    GPSmanager(HardwareSerial* port, DateTime* now, DataQueue* logQ, DataQueue* transmitQ);
    void initialize();
    void run();
  private:
    HardwareSerial* port;
    FlightGPS* gps;
    DataQueue* logQ,* transmitQ;
    class GPSlogAction: public RepeatingAction {
      public:
        GPSlogAction(unsigned long logTime, FlightGPS* gps, DataQueue* logQ, DataQueue* transmitQ);
      private:
        void execute();
        FlightGPS* gps;
        DataQueue* logQ,* transmitQ;
    };
    GPSlogAction* gpsLogger;
};

#endif
