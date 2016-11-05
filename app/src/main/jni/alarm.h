//
// Created by johan on 2016/10/31.
//

#ifndef MY_APPLICATION_ALARM_H
#define MY_APPLICATION_ALARM_H

#endif //MY_APPLICATION_ALARM_H

struct timer_struct {
  int remainingCycles;
  int started;
  void (*expiredevent) (struct timer_struct*);
  void (*interrupt) (struct timer_struct*);
  int stateParam1;
  int stateParam2;
  int stateParam3;
};

void add_timer_to_list(struct timer_struct * timer);