//
// Created by johan on 2016/10/31.
//

#include <alarm.h>


void expired(struct timer_struct *tdev) {
  tdev->started=0;
}

struct timer_struct getTimerInstance() {
  struct timer_struct mytimer;
  mytimer.expiredevent = &expired;
}