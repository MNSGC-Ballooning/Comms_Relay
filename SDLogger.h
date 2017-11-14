#ifndef SDLogger_h
#define SDLogger_h

#include <SD.h>
#include "System.h"

class SDLogger: public System {
  public:
    SDLogger(byte chipSelect, DateTime* now, DataQueue* gpsQ, DataQueue* radioQ);
    void initialize();
    void run();
  private:
    byte chipSelect;
    char folder[31];
    DataQueue* gpsQ,* radioQ;
    const String gpsFile = "gpslog.csv", radioFile = "radiolog.csv";
};

#endif
