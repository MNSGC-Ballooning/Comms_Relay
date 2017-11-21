#ifndef Data_h
#define Data_h

#include <Arduino.h>

class Data {
  public:
    virtual ~Data(){}
    virtual void toCharArray(char* data) = 0;
};

class GPSdata: public Data {
  public:
    GPSdata(byte hour, byte minute, byte second, float lat, float lon, float alt, byte sats);
    void toCharArray(char* data);
  private:
    byte hour, minute, second, sats;
    float lat, lon, alt;
};

class DateTime: public Data {
  public:
    DateTime(int year, byte month, byte day, byte hour, byte minute, byte second);
    void toCharArray(char* data);
  private:
    byte dateTime[6];
};

class RadioData: public Data {
  public:
    RadioData(DateTime* time, char source, String message);
    void toCharArray(char* data);
  private:
    char time[8];
    char source;
    String message;
};

class DataQueue {
  public:
    void push(Data* elem);
    Data* pop();
  private:
    class Node {
      public:
        Node(Data* elem);
        Data* elem;
        Node* next = NULL;
    };
    Node* head;
};

#endif
