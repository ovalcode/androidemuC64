//
// Created by johan on 2016/10/31.
//

#include <alarm.h>


void expired(struct timer_struct *tdev) {
  tdev->started=0;
}

struct timer_struct getTimerInstanceA() {
  struct timer_struct mytimer;
  mytimer.expiredevent = &expired;
  return mytimer;
}

struct timer_struct getTimerInstanceB() {
  struct timer_struct mytimer;
  mytimer.expiredevent = &expired;
  return mytimer;
}