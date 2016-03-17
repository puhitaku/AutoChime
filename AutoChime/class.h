#pragma once
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Servo.h>
#include <Time.h>
#include <TickerScheduler.h>
#include <vector>
using namespace std;


typedef unsigned char uchar;
typedef unsigned long ulong;
typedef unsigned int uint;

enum class Serv { Big, Small };
enum class Tone { Low, Mid, High };

class Note {
  public:
    Note(Tone _tone, int _duration);
    void SetShortOrNot(bool _is_short);
    Tone GetTone();
    int GetDuration();
    bool GetIsShort();
  protected:
    Tone tone;
    int duration;
    bool is_short;
};

class NoteL : public Note { public: NoteL(Tone _tone, int _duration); };
class NoteS : public Note { public: NoteS(Tone _tone, int _duration); };

class Score {
  public:
    Score(vector<Note>);
    vector<Note> notes;

    int GetScoreLength();
    int GetTimeLength();
};

class Chime {
  public:
    Chime(char pin1, char pin2);
    void SetPinNumber(char pin1, char pin2);
    void SetChimeDirection(int pin1, int pin2, int pin3);
    void SetHammerDirection(int touch, int release);
    void InitHammer();

    void Attach(Serv servo_id);
    void Detach(Serv servo_id);
    void AttachAll();
    void DetachAll();
    void MoveServo(Serv servo_id, int us);
    void Ring(Tone tone, bool is_short, bool wait_for_move);

    void PlayScore(int score_index, int delay_before_ms);
    void AddWork(char hour, char minute, char second, Score score);

    void TsWaitLong(TickerScheduler& ts);
    void TsWaitShort(TickerScheduler& ts);
    int isIncoming(int prior_sec);

  private:
    array<Servo, 2> servo_;
    array<char, 2> servo_pin_;
    array<int, 3> chime_dir_;
    vector<time_t> work_time_;
    vector<Score> score_book_;
    int hammer_dir_touch_, hammer_dir_release_;
    time_t GetUnbiasedTime(int hour, int minute, int second);
};

class TimeNTP : public ESP8266WiFiClass {
  public:
    TimeNTP() {};
    bool BeginWiFi(const char* ssid, const char* pass, bool debug);
    void BeginNtp(const char* ntp_server, int ntp_port, bool debug);
    void SetTimeZone(char tz);
    time_t GetNtpTime(int timezone, bool debug);
    ulong SendNtpRequest(IPAddress address);

  private:
    const char NTP_PACKET_LENGTH = 48;
    IPAddress ntp_ipaddr;
    WiFiUDP udp;
    byte pbuf[48];
};

