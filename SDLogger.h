#ifndef SDLogger_h
#define SDLogger_h

#include <SD.h>
#include "System.h"

//Class to handle logging data to SD card
class SDLogger: public System {
  public:
    SDLogger(byte chipSelect, DateTime* now, DataQueue* gpsQ, DataQueue* radioQ);
    void initialize();
    void run();
  private:
    byte chipSelect; //CS pin for SPI connection
    char folder[30]; //name of file and folder to log data to
    DataQueue* gpsQ,* radioQ; //separate queues for gps data and radio data
    const String gpsFile = "gpslog.csv", radioFile = "radlog.csv"; //common names for flight logs
};

#endif
