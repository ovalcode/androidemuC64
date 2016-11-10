//
// Created by johan on 2016/10/31.
//

#include <alarm.h>
#include <interrupts.h>
#include <android/log.h>

void expired(struct timer_struct *tdev) {
  __android_log_print(ANDROID_LOG_DEBUG, "expired", "expired");
  tdev->remainingCycles = tdev->stateParam1;
  if (tdev->stateParam1 == 0) //if not continuios
    tdev->started=0;
  tdev->interrupt();
}

void set_timer_low(struct timer_struct *tdev , int lowValue) {
  __android_log_print(ANDROID_LOG_DEBUG, "set timer low", "set timer low");
  tdev->stateParam2 = tdev->stateParam2 & (0xff << 8);
  tdev->stateParam2 = tdev->stateParam2 | lowValue;
}

void set_timer_high(struct timer_struct *tdev , int highValue) {
  __android_log_print(ANDROID_LOG_DEBUG, "set timer high", "set timer high");
  tdev->stateParam2 = tdev->stateParam2 & 0xff;
  tdev->stateParam2 = tdev->stateParam2 | (highValue << 8);
}

int get_time_low(struct timer_struct *tdev) {
  int low = tdev->remainingCycles & 0xff;
  return low;
}

int get_control_reg(struct timer_struct *tdev) {
  int value = 0;

  if(tdev->started==1)
    value = value | 1;

  if(tdev->stateParam1==0)
    value = value | 8;

  return value;
}

void set_control_reg(struct timer_struct *tdev, int value) {
  tdev->started = value & 1;
  tdev->stateParam1 = ((value & 8) == 8) ? 0 : 1;

  if ((value & 16) != 0)
    tdev->remainingCycles = tdev->stateParam2;
}


int get_time_high(struct timer_struct *tdev) {
  int high = tdev->remainingCycles & (0xff<<8);
  high = high >> 8;
  return high;
}

struct timer_struct getTimerInstanceA() {
  struct timer_struct mytimer;
  mytimer.expiredevent = &expired;
  mytimer.remainingCycles = 0xffff;
  mytimer.started = 0;
  mytimer.interrupt = &interrupt_timer_A;
  return mytimer;
}

struct timer_struct getTimerInstanceB() {
  struct timer_struct mytimer;
  mytimer.expiredevent = &expired;
  mytimer.remainingCycles = 0xffff;
  mytimer.started = 0;
  mytimer.interrupt = &interrupt_timer_B;
  return mytimer;
}
