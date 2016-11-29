//
// Created by johan on 2016/11/29.
//

#ifndef ANDROIDEMUC64_TAPE_H_H
#define ANDROIDEMUC64_TAPE_H_H

#endif //ANDROIDEMUC64_TAPE_H_H

int getMotorOnBit(struct timer_struct *tdev);
int isPlayDownBit();
void setMotorOn(struct timer_struct *tdev, int motorBit);
struct timer_struct getTapeInstance();
void attachNewTape(jbyte* buffer, struct timer_struct *tdev);