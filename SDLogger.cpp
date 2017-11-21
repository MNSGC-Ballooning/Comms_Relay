#include "SDLogger.h"

SDLogger::SDLogger(byte chipSelect, DateTime* now, DataQueue* gpsQ, DataQueue* radioQ):
  System(now), chipSelect(chipSelect), gpsQ(gpsQ), radioQ(radioQ) {
}

void SDLogger::initialize() {
  now->toCharArray(folder);
  folder[11] = '_';
  folder[14] = '_';
  folder[17] = '/';
  pinMode(chipSelect, OUTPUT);
  SD.begin(chipSelect);
  if (!SD.exists(folder)) SD.mkdir(folder);
  gpsFile.toCharArray(folder+18, 13);
  File file = SD.open(folder, FILE_WRITE);
  file.println("Time,Lat,Lon,Alt,Sats");
  file.close();
  radioFile.toCharArray(folder+18, 13);
  file = SD.open(folder, FILE_WRITE);
  file.println("Time,Source,Message");
  file.close();
}

void SDLogger::run() {
  Data* logData = gpsQ->pop();
  while (logData) {
    gpsFile.toCharArray(folder+18, 13);
    File file = SD.open(folder, FILE_WRITE);
    char data[42] = {0};
    logData->toCharArray(data);
    file.println(String(data) + ',');
    file.close();
    delete logData; logData = NULL;
    logData = gpsQ->pop();
  }
  logData = radioQ->pop();
  while (logData) {
    radioFile.toCharArray(folder+18, 13);
    File file = SD.open(folder, FILE_WRITE);
    char data[42] = {0};
    now->toCharArray(data);
    logData->toCharArray(data);
    file.println(String(data) + ',');
    file.close();
    delete logData;
    logData = radioQ->pop();
  }
}

