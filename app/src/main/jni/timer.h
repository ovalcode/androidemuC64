//
// Created by johan on 2016/11/09.
//

#ifndef ANDROIDEMUC64_TIMER_H
#define ANDROIDEMUC64_TIMER_H

#endif //ANDROIDEMUC64_TIMER_H

void set_timer_low(struct timer_struct *tdev , int lowValue);
void set_timer_high(struct timer_struct *tdev , int highValue);
int get_time_low(struct timer_struct *tdev);
int get_control_reg(struct timer_struct *tdev);
void set_control_reg(struct timer_struct *tdev, int value);
int get_time_high(struct timer_struct *tdev);
struct timer_struct getTimerInstanceA();
struct timer_struct getTimerInstanceB();
