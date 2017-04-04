//
// Created by johan on 2016/10/31.
//

#ifndef MY_APPLICATION_ALARM_H
#define MY_APPLICATION_ALARM_H

#endif //MY_APPLICATION_ALARM_H

typedef struct timer_struct {
  int targetClock;
  int started;
  void (*expiredevent) (struct timer_struct*, int clock);
  void (*interrupt) ();
} timer_struct;

void add_timer_to_list(timer_struct * timer);