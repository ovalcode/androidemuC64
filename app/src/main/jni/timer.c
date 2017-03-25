//
// Created by johan on 2016/10/31.
//

#include <alarm.h>
#include <interrupts.h>
#include <android/log.h>
#include <stdint.h>
#include "timer.h"

void decrement_underflow_count (cia_timer_struct *tdev) {
  if (tdev->count_mode == 0)
    return;
  tdev->timer.remainingCycles = tdev->timer.remainingCycles -1;
  if (tdev->timer.remainingCycles < 0) {
    tdev->timer.remainingCycles = tdev->latch_value;
    if (tdev->continious == 0) //if not continuios
      tdev->underflow_counting_started=0;
  }
}

void expired(cia_timer_struct *tdev) {
  //__android_log_print(ANDROID_LOG_DEBUG, "expired", "expired");
  tdev->timer.remainingCycles = tdev->latch_value;
  if (tdev->continious == 0) //if not continuios
    tdev->timer.started=0;
  if (tdev->linked_timer != NULL) {
    decrement_underflow_count(tdev->linked_timer);
  }
  tdev->timer.interrupt();
}


void set_timer_low(cia_timer_struct *tdev , int lowValue) {
  //__android_log_print(ANDROID_LOG_DEBUG, "set timer low", "set timer low");
  tdev->latch_value = tdev->latch_value & (0xff << 8);
  tdev->latch_value = tdev->latch_value | lowValue;
}

void set_timer_high(cia_timer_struct *tdev , int highValue) {
  //__android_log_print(ANDROID_LOG_DEBUG, "set timer high", "set timer high");
  tdev->latch_value = tdev->latch_value & 0xff;
  tdev->latch_value = tdev->latch_value | (highValue << 8);
}

int get_time_low(cia_timer_struct *tdev) {
  int low = tdev->timer.remainingCycles & 0xff;
  return low;
}

int get_control_reg_02(cia_timer_struct *tdev) {
  int value = 0;

  if(tdev->timer.started==1)
    value = value | 1;

  if(tdev->continious==0)
    value = value | 8;

  return value;

}

int get_control_reg_underflow(cia_timer_struct *tdev) {
  int value = 0;

  if(tdev->underflow_counting_started==1)
    value = value | 1;

  if(tdev->continious==0)
    value = value | 8;

  return value;

}

int get_control_reg(cia_timer_struct *tdev) {
  if (tdev->count_mode == 0) {
    return get_control_reg_02(tdev);
  } else {
    return get_control_reg_underflow(tdev);
  }

}

void set_control_reg_02(cia_timer_struct *tdev, int value) {
  tdev->timer.started = value & 1;
  tdev->continious = ((value & 8) == 8) ? 0 : 1;

  if ((value & 16) != 0)
    tdev->timer.remainingCycles = tdev->latch_value;
}

void set_control_reg_Underflow(cia_timer_struct *tdev, int value) {
  tdev->underflow_counting_started = value & 1;
  tdev->timer.started = 0;
  tdev->continious = ((value & 8) == 8) ? 0 : 1;

  if ((value & 16) != 0)
    tdev->timer.remainingCycles = tdev->latch_value;
}

void set_control_reg(cia_timer_struct *tdev, int value) {
  if (tdev->timer_type == 0) {
    set_control_reg_02(tdev, value);
  } else {
    if (value & 0x40) {
      tdev->count_mode = 1;
      set_control_reg_Underflow(tdev, value);
    }  else {
      tdev->count_mode = 0;
      set_control_reg_02(tdev, value);
    }
  }


}


int get_time_high(cia_timer_struct *tdev) {
  int high = tdev->timer.remainingCycles & (0xff<<8);
  high = high >> 8;
  return high;
}

cia_timer_struct getTimerInstanceA() {
  cia_timer_struct mytimer;
  mytimer.timer.expiredevent = &expired;
  mytimer.timer.remainingCycles = 0xffff;
  mytimer.timer.started = 0;
  mytimer.timer.interrupt = &interrupt_timer_A;
  mytimer.timer_type = 0;
  mytimer.linked_timer = NULL;
  mytimer.count_mode = 0;
  mytimer.underflow_counting_started = 0;
  return mytimer;
}

cia_timer_struct getTimerInstanceB() {
  cia_timer_struct mytimer;
  mytimer.timer.expiredevent = &expired;
  mytimer.timer.remainingCycles = 0xffff;
  mytimer.timer.started = 0;
  mytimer.timer.interrupt = &interrupt_timer_B;
  mytimer.timer_type = 1;
  mytimer.linked_timer = NULL;
  mytimer.count_mode = 0;
  mytimer.underflow_counting_started = 0;
  return mytimer;
}
