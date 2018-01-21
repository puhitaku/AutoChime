import time
import network
from machine import Pin, PWM, RTC

import ntptime_kai as ntptime

rtc = RTC()


class TimeKeeper:
    class Task:
        def __init__(self, hour, minute, fn, offset=0):
            self.hour, self.minute = hour, minute
            self.fn = fn
            self.offset = offset

    def __init__(self):
        self.tasks = []

    def add_task(self, hour, minute, fn, offset=0):
        self.tasks.append(self.Task(hour, minute, fn, offset))

    def run(self, syncer=None, sync_duration=3600):
        syncer()
        last_sync = time.time()

        while True:
            if time.time() - last_sync > sync_duration:
                syncer()
                last_sync = time.time()

            self._check_and_do()
            time.sleep(1)

    def _check_and_do(self):
        now = time.localtime()
        now_u = time.time()
        print('Now    : {}'.format(now))

        for i, task in enumerate(self.tasks):
            t = (now[0], now[1], now[2], task.hour, task.minute, 0, 0, 0)
            d = time.mktime(t) - time.mktime(now)
            print('Next {i:02d}: {t}\nDiff {i:02d}:{d}'.format(i=i+1, t=t, d=d))

            if d >= 0 and d < 10:
                print('Triggering!')
                while True:
                    now_u = time.time()
                    d = time.mktime(t) - now_u
                    print('Diff : {}'.format(d))

                    if d + task.offset == 0:
                        task.fn()
                        break
                    time.sleep_ms(10)
        print('')


class ServoController:
    _pwm_freq = 200

    _tone_duty = {
        'l': 342,
        'm': 320,
        'h': 293}

    _mallet_duty = {
        'd': 265,
        'u': 350}

    _servo_tone = None
    _servo_mallet = None

    def __init__(self, pin_tone, pin_mallet):
        self._servo_tone = PWM(Pin(pin_tone), freq=self._pwm_freq)
        self._servo_mallet = PWM(Pin(pin_mallet), freq=self._pwm_freq)

    def ring(self, tone, move=True, raise_time=0.12):
        if move:
            self.rot_tone(tone)
        time.sleep(0.7)

        self.rot_mallet('d')
        time.sleep(raise_time)
        self.rot_mallet('u')
        time.sleep(0.3 - raise_time)

    def rot_tone(self, tone):
        self._servo_tone.duty(self._tone_duty[tone])
        time.sleep(0.05)
        self._servo_tone.deinit()

    def rot_mallet(self, state):
        self._servo_mallet.duty(self._mallet_duty[state])

    def init(self):
        self._servo_tone.init()
        self._servo_mallet.init()

    def deinit(self, servo=None):
        if servo is None:
            self._servo_tone.deinit()
            self._servo_mallet.deinit()
        elif servo == 'tone':
            self._servo_tone.deinit()
        elif servo == 'mallet':
            self._servo_mallet.deinit()

    def _use_servo(fn):
        def inner(self):
            self.init()
            self.rot_mallet('u')
            fn(self)
            self.deinit()
        return inner

    @_use_servo
    def morning(self):
        self.ring('m')
        self.ring('m', move=False)
        self.ring('m', move=False)
        self.ring('h')

    @_use_servo
    def finish1(self):
        self.ring('m')

    @_use_servo
    def finish2(self):
        self.ring('h')


def connect_wifi(essid, psk, ifconfig=False):
    sta = network.WLAN(network.STA_IF)
    sta.active(True)
    sta.connect(essid, psk)
    while not sta.isconnected():
        time.sleep(0.5)

    if ifconfig:
        c = sta.ifconfig()
        print('Addr={}, Mask={}, GW={}, DNS={}'.format(*c))
    return sta


def sync_with_ntp():
    sta = connect_wifi('ESSID', 'PSK')
    ntptime.host = 'ntp.nict.jp'
    ntptime.settime(ms_accuracy=True)
    sta.disconnect()
    sta.active(False)


def main():
    s = ServoController(13, 5)

    t = TimeKeeper()
    t.add_task(1, 0, s.morning, offset=-4)
    t.add_task(6, 0, s.finish1, offset=-1)
    t.add_task(9, 0, s.finish2, offset=-1)
    t.run(syncer=sync_with_ntp)


while True:
    main()

