#include <Servo.h>
#include "class.h"

Chime::Chime(char pin1, char pin2) {
  SetPinNumber(pin1, pin2);
}

void Chime::SetPinNumber(char pin1, char pin2) {
  servo_pin_[(char)Serv::Big] = pin1;
  servo_pin_[(char)Serv::Small] = pin2;
}

void Chime::SetChimeDirection(int pos1, int pos2, int pos3) {
  chime_dir_[0] = pos1;
  chime_dir_[1] = pos2;
  chime_dir_[2] = pos3;
}

void Chime::SetHammerDirection(int touch, int release) {
  hammer_dir_touch_ = touch;
  hammer_dir_release_ = release;
  MoveServo(Serv::Small, hammer_dir_release_);
}

void Chime::InitHammer() {
  AttachAll();
  MoveServo(Serv::Big, chime_dir_[(char)Tone::Mid]);
  MoveServo(Serv::Small, hammer_dir_release_);
  DetachAll();
}

void Chime::Attach(Serv servo_id) {
  char pin = servo_pin_[(char)servo_id];
  servo_[(char)servo_id].attach(pin);
  delay(100);  //100ms is needed at least
}

void Chime::AttachAll() {
  Attach(Serv::Small);
  Attach(Serv::Big);
}

void Chime::Detach(Serv servo_id) {
  servo_[(char)servo_id].detach();
}

void Chime::DetachAll() {
  Detach(Serv::Small);
  Detach(Serv::Big);
}

void Chime::MoveServo(Serv servo_id, int us) {
  servo_[(char)servo_id].writeMicroseconds(us);
}

void Chime::Ring(Tone tone, bool is_short, bool wait_for_move) {
  MoveServo(Serv::Big, chime_dir_[(char)tone]);
  delay(200);
  MoveServo(Serv::Small, hammer_dir_touch_);
  if(is_short) {
    delay(500);
    MoveServo(Serv::Small, hammer_dir_release_);
    delay(300);
  } else {
    delay(230);
    MoveServo(Serv::Small, hammer_dir_release_);
    delay(570);
  }
}

void Chime::PlayScore(int score_index) {
  int play_time = score_book_[score_index].GetTimeLength() * 1050;
  delay(5000 - play_time - 100 + 500);  //-100ms = Attach time, 500ms = Adjustment
  AttachAll();
  for(auto note: score_book_[score_index].notes) {
    Serial.printf("Tone: %d, IsShort: %d\n", note.GetTone(), note.GetIsShort());
    Ring(note.GetTone(), note.GetIsShort(), false);
  }
  DetachAll();
}

void Chime::AddWork(char hour, char minute, char second, Score score) {
  work_time_.push_back(GetUnbiasedTime(hour, minute, second));
  score_book_.push_back(score);
}

void Chime::TsWaitLong(TickerScheduler& ts) {
  int index = chime.isIncoming(15);
  if(index >= 0) {
    ts.add(1, 100, [&]{ TsWaitShort(ts); }, true);
  }
}

void Chime::TsWaitShort(TickerScheduler& ts) {
  int index = chime.isIncoming(5);
  if(index >= 0) {
    ts.remove(0);
    ts.remove(1);
    chime.PlayScore(index);
    ts.add(0, 5000, [&]{ TsWaitLong(ts); }, false);
  }
}

int Chime::isIncoming(int prior_sec) {
  for(int i=0; i<work_time_.size(); i++) {
    long int diff = GetUnbiasedTime(hour(), minute(), second()) - work_time_[i];
    TimeElements tm;
    breakTime(work_time_[i], tm);
    Serial.printf("%02d:%02d:%02d\n", hour(), minute(), second());
    Serial.printf("Work[%d]: %d:%d:%d, ", i, tm.Hour, tm.Minute, tm.Second);
    Serial.printf("diff = %d[sec]\n", diff);
    if(-prior_sec <= diff && diff <= 0) {
      return i;
    }
  }
  Serial.println();
  return -1;
}

time_t Chime::GetUnbiasedTime(int hour, int minute, int second) {
  TimeElements tm;
  tm.Second = second; tm.Minute = minute; tm.Hour = hour;
  tm.Day = tm.Month = 1; tm.Year = 39;
  return makeTime(tm);
}
