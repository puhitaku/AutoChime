import ntptime
import time
import network

from machine import Pin, PWM, RTC


class ServoController:
    _pwm_freq = 200
    _pwm_duty_start = 100
    _pwm_duty_end = 500

    _tone_duty = {
        'l': 342,
        'm': 320,
        'h': 300}

    _mallet_duty = {
        'd': 270,
        'u': 350}

    _servo_tone = None
    _servo_mallet = None

    def __init__(self, pin_tone, pin_mallet):
        self._servo_tone = PWM(Pin(pin_tone), freq=self._pwm_freq)
        self._servo_mallet = PWM(Pin(pin_mallet), freq=self._pwm_freq)

    def ring(self, tone, move=True, deinit_time=0.1):
        if move:
            self._send(self._servo_tone, self._tone_duty[tone])
            time.sleep(deinit_time)
            self._servo_tone.deinit()
            time.sleep(0.5)
        else:
            time.sleep(0.55)

        self._send(self._servo_mallet, self._mallet_duty['d'])
        time.sleep(0.15)
        self._send(self._servo_mallet, self._mallet_duty['u'])
        
        time.sleep(0.3)

    def rot_tone(self, tone):
        self._send(self._servo_tone, self._tone_duty[tone])
        time.sleep(0.05)
        self._servo_tone.deinit()

    def rot_mallet(self, state):
        self._send(self._servo_mallet, self._mallet_duty[state])

    def deinit(self, servo=None):
        if servo is None:
            self._servo_tone.deinit()
            self._servo_mallet.deinit()
        elif servo == 'tone':
            self._servo_tone.deinit()
        elif servo == 'mallet':
            self._servo_mallet.deinit()

    def morning(self):
        s.ring('m')
        s.ring('m', deinit_time=0.02)
        s.ring('m', move=False)
        s.ring('h')
        self.deinit()

    @staticmethod
    def _send(servo, pos):
        servo.duty(pos)

    @classmethod
    def _calc_servo_duration(cls, start, end):
        diff = abs(end - start)
        return diff / (cls._pwm_duty_end - cls._pwm_duty_start) * 0.6


def connect_wifi(essid, psk):
    sta = network.WLAN(network.STA_IF)
    sta.active(True)
    sta.connect(essid, psk)
    while not sta.isconnected():
        time.sleep(0.5)
    c = sta.ifconfig()
    print('Addr={}, Mask={}, GW={}, DNS={}'.format(*c))


def sync_with_ntp(host):
    ntptime.host = host
    rtc = RTC()
    rtc.datetime(ntptime.time())


def main():
    connect_wifi('ESSID', 'PSK')
    sync_with_ntp('ntp.nict.jp')

    s = ServoController(13, 5)
    s.morning()

