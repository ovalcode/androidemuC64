//
// Created by johan on 2016/10/31.
//

#include <alarm.h>


void expired(struct timer_struct *tdev) {
  tdev->remainingCycles = tdev->stateParam1;
  if (tdev->stateParam1 == 0) //if not continuios
    tdev->started=0;
}

struct timer_struct getTimerInstanceA() {
  struct timer_struct mytimer;
  mytimer.expiredevent = &expired;
  mytimer.RemainingCycles = 0xffff;
  mytimer.started = 0;
  return mytimer;
}

struct timer_struct getTimerInstanceB() {
  struct timer_struct mytimer;
  mytimer.expiredevent = &expired;
  mytimer.RemainingCycles = 0xffff;
  mytimer.started = 0;
  return mytimer;
}
