#include "SDLogger.h"

#define HardwareChipSelect 53

SDLogger::SDLogger(byte chipSelect, DateTime* now, DataQueue* gpsQ, DataQueue* radioQ):
  System(now), chipSelect(chipSelect), gpsQ(gpsQ), radioQ(radioQ) {
}

void SDLogger::initialize() {
  now->toCharArray(folder);
  folder[17] = '/';
  pinMode(chipSelect, OUTPUT);
  if (chipSelect != HardwareChipSelect) pinMode(HardwareChipSelect, OUTPUT);
  SD.begin(chipSelect);
  SD.mkdir(folder);
  gpsFile.toCharArray(folder+18, 13);
  File file = SD.open(folder);
  file.println("Time,Lat,Lon,Alt,Sats");
  file.close();
  radioFile.toCharArray(folder+18, 13);
  file = SD.open(folder);
  file.println("Time,Source,Message");
  file.close();
}

void SDLogger::run() {
  Data* logData = gpsQ->pop();
  while (logData != NULL) {
    gpsFile.toCharArray(folder+18, 13);
    File file = SD.open(folder);
    char data[42] = {0};
    logData->toCharArray(data);
    file.println(String(data) + ',');
    file.close();
    delete logData;
    logData = gpsQ->pop();
  }
  logData = radioQ->pop();
  while (logData != NULL) {
    radioFile.toCharArray(folder+18, 13);
    File file = SD.open(folder);
    char data[42] = {0};
    now->toCharArray(data);
    for (byte i = 0; i < 9; i++) {
      data[i] = data[i+9];
      data[i+9] = 0;
    }
    data[9] = ',';
    logData->toCharArray(data + 10);
    file.println(String(data) + ',');
    file.close();
    delete logData;
    logData = radioQ->pop();
  }
}

