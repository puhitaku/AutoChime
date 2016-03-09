#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Time.h>
#include "class.h"


bool TimeNTP::BeginWiFi(const char* ssid, const char* pass, bool debug) {
  if(debug) {
    Serial.println("Setting AP Mode.");
    WiFi.mode(WIFI_STA);
    Serial.printf("Connecting to %s\n", ssid);
  }
  this -> begin(ssid, pass);
  while(this -> status() != WL_CONNECTED) {
    delay(100);
    if(debug) { Serial.print("."); }
  }
  if(debug) { Serial.println("Connected successfully."); }
}

void TimeNTP::BeginNtp(const char* ntp_server, int ntp_port, bool debug) {
  udp.begin(ntp_port);
  this -> hostByName(ntp_server, ntp_ipaddr); 
}

time_t TimeNTP::GetNtpTime(int timezone, bool debug) {
  while(udp.parsePacket() > 0);
  SendNtpRequest(ntp_ipaddr);
  uint32_t start = millis();

  while(millis() - start < 1500) {
    delay(1);
    int psize = udp.parsePacket();
    if(psize >= NTP_PACKET_LENGTH) {
      udp.read(pbuf, NTP_PACKET_LENGTH);
      ulong sec;
      sec  = (ulong)pbuf[40] << 24;
      sec |= (ulong)pbuf[41] << 16;
      sec |= (ulong)pbuf[42] << 8;
      sec |= (ulong)pbuf[43];
      return sec - 2208988800UL + timezone * SECS_PER_HOUR;
    }
  }
  return 0;
}

ulong TimeNTP::SendNtpRequest(IPAddress address) {
  memset(pbuf, 0, NTP_PACKET_LENGTH);
  pbuf[0]  = 0xE3;
  pbuf[1]  = 0;
  pbuf[2]  = 6;
  pbuf[3]  = 0xEC;
  pbuf[12] = 49;
  pbuf[13] = 0x4E;
  pbuf[14] = 49;
  pbuf[15] = 52;

  udp.beginPacket(address, 123);
  udp.write(pbuf, NTP_PACKET_LENGTH);
  udp.endPacket();
}

