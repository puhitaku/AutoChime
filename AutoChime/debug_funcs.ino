#include "class.h"
#include "score_book.h"
using namespace std;


void testServo(Chime chime, Serv servo_id) {
  int v = 0;
  while(v != -1) {
    v = WaitParseInt(Serial);
    Serial.printf("Sending %d[us]...\n", v);
    chime.MoveServo(servo_id, v);
  }
}

void testTone(bool isShort) {
  int v = 0;
  while(v != -1) {
    v = WaitParseInt(Serial);
    Serial.printf("Ringing tone %d...\n", v);
    chime.Ring(Tone::Mid, isShort, true);
  }
}

void testScore(Chime chime) {
  chime.PlayScore(0, 0);
}

int WaitParseInt(HardwareSerial& serial) {
  while(serial.available() == 0) {
    delay(1);
  }
  return serial.parseInt();
}