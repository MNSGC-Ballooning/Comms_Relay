#include "Data.h"

GPSdata::GPSdata(byte hour, byte minute, byte second, float lat, float lon, float alt, byte sats):
  hour(hour), minute(minute), second(second), lat(lat), lon(lon), alt(alt), sats(sats) {
}

void GPSdata::toCharArray(char* data) {
  byte pos = 0;
  String str = String(hour);
  for (byte i = str.length(); i < 2; i++, pos++) {
    data[pos] = '0';
  }
  str.toCharArray(data + pos, str.length() + 1);
  pos += str.length();

  data[pos] = ':';
  pos++;

  str = String(minute);
  for (byte i = str.length(); i < 2; i++, pos++) {
    data[pos] = '0';
  }

  str.toCharArray(data + pos, str.length() + 1);
  pos += str.length();

  data[pos] = ':';
  pos++;

  str = String(second);
  for (byte i = str.length(); i < 2; i++, pos++) {
    data[pos] = '0';
  }

  str.toCharArray(data + pos, str.length() + 1);
  pos += str.length();

  data[pos] = ',';
  pos++;

  str = String(lat, 4);
  str.toCharArray(data + pos, str.length() + 1);
  pos += str.length();

  data[pos] = ',';
  pos++;

  str = String(lon, 4);
  str.toCharArray(data + pos, str.length() + 1);
  pos += str.length();

  data[pos] = ',';
  pos++;

  str = String(alt, 1);
  str.toCharArray(data + pos, str.length() + 1);
  pos += str.length();

  data[pos] = ',';
  pos++;

  str = String(sats);
  str.toCharArray(data + pos, str.length() + 1);
  pos += str.length();
}

DateTime::DateTime(int year, byte month, byte day, byte hour, byte minute, byte second):
  dateTime{year%100, month, day, hour, minute, second} {
}

void DateTime::toCharArray(char* data) {
  for (byte dataPos = 0, i = 0; i < 6; i++) {
    data[dataPos] = '0' + dateTime[i]/10;
    dataPos++;
    data[dataPos] = '0' + dateTime[i]%10;
    dataPos++;
    if (i == 5) break;
    else if (i == 2) data[dataPos] = '/';
    else if (i < 2) data[dataPos] = '-';
    else data[dataPos] = ':';
    dataPos++;
  }
}

RadioData::RadioData(DateTime* time, char source, String message):
  source(source), message(message) {
    char fullTime[18];
    time->toCharArray(fullTime);
    for (byte i = 0; i < 8; i++) {
      this->time[i] = fullTime[i+9];
    }
}

void RadioData::toCharArray(char* data) {
  for (byte i = 0; i < 8; i++) {
    data[i] = time[i];
  }
  data[8] = ',';
  data[9] = source;
  data[10] = ',';
  message.toCharArray(data+11, message.length()+1);
}

void DataQueue::push(Data* elem) {
  if (head == NULL) {
    head = new Node(elem);
  }
  else {
    Node* runner;
    for (runner = head; runner->next != NULL; runner = runner->next) {}
    runner->next = new Node(elem);
  }
}

Data* DataQueue::pop() {
  if (head == NULL) return NULL;
  Data* top = head->elem;
  Node* toDelete = head;
  head = head->next;
  delete toDelete;
  return top;
}

DataQueue::Node::Node(Data* elem) {
  this->elem = elem;
}

