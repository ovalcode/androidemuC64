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
  tdev->kickOffCount = tdev->kickOffCount -1;
  if (tdev->kickOffCount < 0) {
    tdev->kickOffCount = tdev->latch_value;
    if (tdev->continious == 0) //if not continuios
      tdev->underflow_counting_started=0;
  }
}

int getTotalCycleCount ();
int getCurrentCycles();

void expired(cia_timer_struct *tdev, int rclock) {
  //__android_log_print(ANDROID_LOG_DEBUG, "expired", "expired");
  tdev->kickOffCount = tdev->latch_value /*+ (tdev->timer.targetClock - rclock) + 2*/;
  int remaininfgCycles = tdev->timer.targetClock - rclock;
  int rescheduleAlarm = 1;
  if (tdev->continious == 0) { //if not continuios
    tdev->timer.started=0;
    rescheduleAlarm = 0;
  }
  if (tdev->linked_timer != NULL) {
    decrement_underflow_count(tdev->linked_timer);
  }
  tdev->timer.interrupt();

  if (rescheduleAlarm)
    tdev->timer.targetClock = rclock + tdev->kickOffCount + remaininfgCycles + 1;
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

int getRemainingCycles(cia_timer_struct *tdev) {
  if (!tdev->timer.started)
    return tdev->kickOffCount;
  int temp = tdev->timer.targetClock - getTotalCycleCount() - 2;
  temp = temp - 3 + getCurrentCycles();
  if (temp > tdev->kickOffCount) {
    int numberCyclesMore = temp - tdev->kickOffCount;
    if (numberCyclesMore == 1) {
      return tdev->kickOffCount;
    } else {
      return numberCyclesMore - 1;
    }
  }
  //TODO: subrtact  + add for current
  if (temp < 1) {
    return tdev->latch_value;
  }
  return temp;
}

int get_time_low(cia_timer_struct *tdev) {
  int low = getRemainingCycles(tdev) & 0xff;
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
  int rescheduleAlarm = 0;
  if ((value & 16) != 0) {
    tdev->kickOffCount = tdev->latch_value;
    rescheduleAlarm = 1;
  }

  int oldStarted = tdev->timer.started;
  int newStarted = value & 1;
  if (oldStarted != newStarted) {
    tdev->timer.started = newStarted;
    if (newStarted) {
      rescheduleAlarm = 1;
    } else {
      tdev->kickOffCount = tdev->timer.targetClock - getTotalCycleCount() - 2;
      if (tdev->kickOffCount < 1)
        tdev->kickOffCount = tdev->latch_value;
    }

  }

  if (rescheduleAlarm) {
    tdev->timer.targetClock = getTotalCycleCount() + tdev->kickOffCount + 2 + 2;
  }

  tdev->continious = ((value & 8) == 8) ? 0 : 1;


}

void set_control_reg_Underflow(cia_timer_struct *tdev, int value) {
  tdev->underflow_counting_started = value & 1;
  tdev->timer.started = 0;
  tdev->continious = ((value & 8) == 8) ? 0 : 1;

  if ((value & 16) != 0)
    tdev->kickOffCount = tdev->latch_value;
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
  int high = getRemainingCycles(tdev) & (0xff<<8);
  high = high >> 8;
  return high;
}

cia_timer_struct getTimerInstanceA() {
  cia_timer_struct mytimer;
  mytimer.timer.expiredevent = &expired;
  mytimer.kickOffCount = 0xffff;
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
  mytimer.kickOffCount = 0xffff;
  mytimer.timer.started = 0;
  mytimer.timer.interrupt = &interrupt_timer_B;
  mytimer.timer_type = 1;
  mytimer.linked_timer = NULL;
  mytimer.count_mode = 0;
  mytimer.underflow_counting_started = 0;
  return mytimer;
}
