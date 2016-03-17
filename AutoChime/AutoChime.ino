#include "class.h"
#include "score_book.h"
#include "config.h"
using namespace std;

TimeNTP ntp;
Chime chime(13, 5);
TickerScheduler ts(3);

void setup() {
  Serial.begin(115200);
  
  ntp.BeginWiFi(ssid, pass, true);
  ntp.BeginNtp(ntp_server, 3939, true);

  setSyncProvider([&]{ return ntp.GetNtpTime(timezone, true); });
  setSyncInterval(60 * 60);
  Serial.printf("%d/%02d/%02d ", year(), month(), day());
  Serial.printf("%02d:%02d:%02d\n", hour(), minute(), second());
  delay(500);

  chime.SetChimeDirection(1460, 1560, 1680);
  chime.SetHammerDirection(975, 1510);
  chime.InitHammer();

  chime.AddWork(10, 0, 0, time_signal);
  chime.AddWork(15, 0, 0, mid_once);
  chime.AddWork(18, 0, 0, high_once);
  ts.add(0, 5000, [&]{ chime.TsWaitLong(ts); }, true);

  Serial.println("'0': Move Big Servo\n'1': Move Small Servo\n'2': Test long tones");
  Serial.println("'3': Test short tones\n'4': Test melodies");
}

void loop() {
  delay(1);
  ts.update();
  if(Serial.available() > 0) {
    chime.AttachAll();
    delay(100);
    int cmd = Serial.parseInt();
    switch(cmd) {
      case 0:
        Serial.println("Test big servo.");
        testServo(chime, Serv::Big);
        break;
      case 1:
        Serial.println("Test small servo.");
        testServo(chime, Serv::Small);
        break;
      case 2:
        Serial.println("Test long tone.");
        testTone(false);
        break;
      case 3:
        Serial.println("Test short tone.");
        testTone(true);
        break;
      case 4:
        Serial.println("Test Score.");
        testScore(chime);
        break;
    }
    chime.DetachAll();
    Serial.println("Done.");
  }
}

