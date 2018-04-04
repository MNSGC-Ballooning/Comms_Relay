#include "Data.h"

//RawData constructor
RawData::RawData(String data):
  message(data) {
}

void RawData::toCharArray(char* data) {
  message.toCharArray(data, message.length()+1);
}

//GPSdata constructor
GPSdata::GPSdata(byte hour, byte minute, byte second, float lat, float lon, float alt, byte sats):
  hour(hour), minute(minute), second(second), lat(lat), lon(lon), alt(alt), sats(sats) {
}

//Convert GPS data to printable string
void GPSdata::toCharArray(char* data) {
  byte pos = 0;
  
  //Hour
  String str = String(hour);
  for (byte i = str.length(); i < 2; i++, pos++) {
    data[pos] = '0';
  }
  str.toCharArray(data + pos, str.length() + 1);
  pos += str.length();
  data[pos] = ':';
  pos++;
  
  //Minute
  str = String(minute);
  for (byte i = str.length(); i < 2; i++, pos++) {
    data[pos] = '0';
  }
  str.toCharArray(data + pos, str.length() + 1);
  pos += str.length();
  data[pos] = ':';
  pos++;

  //Second
  str = String(second);
  for (byte i = str.length(); i < 2; i++, pos++) {
    data[pos] = '0';
  }
  str.toCharArray(data + pos, str.length() + 1);
  pos += str.length();
  data[pos] = ',';
  pos++;

  //Lattitude
  str = String(lat, 4);
  str.toCharArray(data + pos, str.length() + 1);
  pos += str.length();
  data[pos] = ',';
  pos++;

  //Longitude
  str = String(lon, 4);
  str.toCharArray(data + pos, str.length() + 1);
  pos += str.length();
  data[pos] = ',';
  pos++;

  //Altitude
  str = String(alt, 1);
  str.toCharArray(data + pos, str.length() + 1);
  pos += str.length();
  data[pos] = ',';
  pos++;

  //Satellites
  str = String(sats);
  str.toCharArray(data + pos, str.length() + 1);
  pos += str.length();
} //end GPSdata::toCharArray()

//DateTime constructor - stores data as array of bytes
DateTime::DateTime(int year, byte month, byte day, byte hour, byte minute, byte second):
  dateTime{year%100, month, day, hour, minute, second} {
}

//Converts DateTime to printable string
void DateTime::toCharArray(char* data) {
  for (byte dataPos = 0, i = 0; i < 6; i++) {
    data[dataPos] = '0' + dateTime[i]/10;
    dataPos++;
    data[dataPos] = '0' + dateTime[i]%10;
    dataPos++;
    //add break characters for date and time
    if (i == 5) break;
    else if (i == 2) data[dataPos] = '/';
    else if (i < 2) data[dataPos] = '-';
    else data[dataPos] = ':';
    dataPos++;
  }
}

//RadioData constructor. Source is 'G' or 'X' for ground station or XBee (or some similar convention)
RadioData::RadioData(DateTime* time, char source, String message):
  source(source), message(message) {
    char fullTime[18];
    time->toCharArray(fullTime);
    for (byte i = 0; i < 8; i++) {
      this->time[i] = fullTime[i+9];
    }
}

//Converts RadioData to printable string
void RadioData::toCharArray(char* data) {
  for (byte i = 0; i < 8; i++) {
    data[i] = time[i];
  }
  data[8] = ',';
  if (source == 'G')
    message.toCharArray(data+9, message.length()+1);
  else {
    data[9] = ',';
    message.toCharArray(data+10, message.length()+1);
  }
}

//Adds a data object to a queue via its pointer
void DataQueue::push(Data* elem) {
  if (head == NULL) { //queue is empty, start at head
    head = new Node(elem);
  }
  else { //find end of queue and add to it
    Node* runner;
    for (runner = head; runner->next != NULL; runner = runner->next) {}
    runner->next = new Node(elem);
  }
}

//Removes and returns the data object at the front of the queue
Data* DataQueue::pop() {
  if (head == NULL) return NULL; //queue is empty, nothing to return
  Data* front = head->elem;
  Node* toDelete = head;
  head = head->next;
  delete toDelete;
  return front;
}

//Node constructor
DataQueue::Node::Node(Data* elem) {
  this->elem = elem;
}

