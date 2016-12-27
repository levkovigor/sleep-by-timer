#include <DS3231.h>
#include <avr/sleep.h> 
#include <avr/power.h>

#define WakePin          2

DS3231 clock;
RTCDateTime dt;

void rtcUp() { }

void(*resetFunc) (void) = 0;

void setSleepTimer(int mins) {
  uint8_t hours = mins / 60;
  uint8_t mins_left = mins - hours * 60;
  dt = clock.getDateTime();
  uint8_t hnow = dt.hour;
  uint8_t mnow = dt.minute;
  uint8_t sec = dt.second;
  mnow += mins_left;
  if (mnow >= 60) {
    mnow -= 60;
    hnow += 1;
  }
  hnow += hours;
  if (hnow > 23) {
    hnow -= 24;
  }
  clock.setAlarm1(0, hnow, mnow, sec, DS3231_MATCH_H_M_S);
}


void sleepNow() {
  ADCSRA = 0;
  power_all_disable();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  attachInterrupt(0, rtcUp, LOW);
  sleep_enable();  
  sleep_mode();  
  sleep_disable();
  power_all_enable();
  detachInterrupt(0);
}


void setup() {
  pinMode(WakePin, INPUT_PULLUP);
  Serial.begin(9600);
  clock.begin();
  clock.enableOutput(false);
  clock.armAlarm1(false);
  clock.armAlarm2(false);
  clock.clearAlarm1();
  clock.clearAlarm2();
}

void loop() {
  dt = clock.getDateTime();
  Serial.println(clock.dateFormat("d-m-Y H:i:s - l", dt));
  setSleepTimer(1);
  Serial.println("Sleep");
  delay(100);
  sleepNow();
  Serial.println("Wake UP");
  //resetFunc();
}
