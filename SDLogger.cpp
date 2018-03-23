#include "SDLogger.h"

//SDLogger constructor
SDLogger::SDLogger(byte chipSelect, DateTime* now, DataQueue* gpsQ, DataQueue* radioQ):
  System(now,NULL), chipSelect(chipSelect), gpsQ(gpsQ), radioQ(radioQ) {
}

//setup() code for SDLogger
void SDLogger::initialize() {
  now->toCharArray(folder); //use current date\time as name of folder\subfolder
  folder[11] = '_';
  folder[14] = '_';
  folder[17] = '/';
  pinMode(chipSelect, OUTPUT);
  if (!SD.begin(chipSelect)) { //if SD card doesn't respond, disable Logger to avoid crashing completely
    enabled = false;
    //ADD ERROR MESSAGE/DISPLAY/SIGNAL HERE!!!
    return;
  }
  if (!SD.exists(folder)) SD.mkdir(folder); //create timestamped folder
  
  //create and set up gps datalog
  gpsFile.toCharArray(folder+18, 13);
  File file = SD.open(folder, FILE_WRITE);
  file.println("Time,Lat,Lon,Alt,Sats");
  file.close();
  
  //create and set up radio datalog
  radioFile.toCharArray(folder+18, 13); 
  file = SD.open(folder, FILE_WRITE);
  file.println("Time,Source,Message");
  file.close();
}

//loop() code for SDLogger
void SDLogger::run() {
  //Process all data in gps queue
  for (Data* logData = gpsQ->pop(); logData; logData = gpsQ->pop()) {
    if (enabled) { //only log data if SD card is responsive
      gpsFile.toCharArray(folder+18, 13);
      File file = SD.open(folder, FILE_WRITE);
      char data[42] = {0};
      logData->toCharArray(data);
      file.println(String(data) + ',');
      file.close();
    }
    delete logData; //Even if SD system is disabled, still need to clear out queues as other systems don't know about failure
  }
  //Process all data in radio queue
  for (Data* logData = radioQ->pop(); logData; logData = radioQ->pop()) {
    if (enabled) { //only log data if SD card is responsive
      radioFile.toCharArray(folder+18, 13);
      File file = SD.open(folder, FILE_WRITE);
      char data[42] = {0};
      logData->toCharArray(data);
      file.println(String(data) + ',');
      file.close();
    }
    delete logData; //Even if SD system is disabled, still need to clear out queues as other systems don't know about failure
  }
}

