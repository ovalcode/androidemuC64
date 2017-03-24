//
// Created by johan on 2016/11/09.
//

#ifndef ANDROIDEMUC64_TIMER_H
#define ANDROIDEMUC64_TIMER_H

#endif //ANDROIDEMUC64_TIMER_H

typedef struct cia_timer_struct {
    timer_struct timer;
    int continious;
    int latch_value;
    struct cia_timer_struct * linked_timer;
    int timer_type;//0 -> timer A, 1 -> timer B
    int count_mode;//0 -> count 02, 1 -> count underflows
    int underflow_counting_started;
} cia_timer_struct;

void set_timer_low(cia_timer_struct *tdev , int lowValue);
void set_timer_high(cia_timer_struct *tdev , int highValue);
int get_time_low(cia_timer_struct *tdev);
int get_control_reg(cia_timer_struct *tdev);
void set_control_reg(cia_timer_struct *tdev, int value);
int get_time_high(cia_timer_struct *tdev);
struct cia_timer_struct getTimerInstanceA();
struct cia_timer_struct getTimerInstanceB();

