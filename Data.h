#ifndef Data_h
#define Data_h

#include <Arduino.h>

//Class to store any data that needs to be transferred between systems
class Data {
  public:
    virtual ~Data(){}
    //Each subclass has own method of converting data to readable form
    virtual void toCharArray(char* data) = 0;
};

//GPS system data
class GPSdata: public Data {
  public:
    GPSdata(byte hour, byte minute, byte second, float lat, float lon, float alt, byte sats);
    void toCharArray(char* data);
  private:
    byte hour, minute, second, sats;
    float lat, lon, alt;
};

//Date and time data - used primarily for timestamping logs
class DateTime: public Data {
  public:
    DateTime(int year, byte month, byte day, byte hour, byte minute, byte second);
    void toCharArray(char* data);
  private:
    byte dateTime[6];
};

//Radio data - used to record transmitted and received messages
class RadioData: public Data {
  public:
    RadioData(DateTime* time, char source, String message);
    void toCharArray(char* data);
  private:
    char time[8];
    char source;
    String message;
};

//Linked data structure to hold Data objects before they're processed
class DataQueue {
  public:
    void push(Data* elem);
    Data* pop();
  private:
    //Class defining elements of a queue
    class Node {
      public:
        Node(Data* elem);
        Data* elem;
        Node* next = NULL;
    };
    Node* head;
};

#endif
