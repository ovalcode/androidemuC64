//
// Created by johan on 2016/11/29.
//
#include <alarm.h>
#include <interrupts.h>
#include <jni.h>

int playDown = 0;
jbyte* tape_image;
int buffer_len;
int posInTape;
int remainingPulse = 0;
int getTotalCycleCount ();

void update_remaining(struct timer_struct *tdev) {
  int temp = tape_image[posInTape];

  if (temp != 0) {
    remainingPulse = temp << 3;
    posInTape++;
  } else {
    remainingPulse = tape_image[posInTape + 1] | (tape_image[posInTape + 2] << 8) | (tape_image[posInTape + 3] << 16);
    posInTape = posInTape + 4;
  }
}

void attachNewTape(jbyte* buffer, int buf_len, struct timer_struct *tdev) {
  tape_image = buffer;
  buffer_len = buf_len;
  posInTape = 0x14;
  update_remaining(tdev);
}

void tape_pulse_expired(struct timer_struct *tdev, int rclock) {
  int remainingCycles = tdev->targetClock - rclock;
  if (posInTape < buffer_len) {
    update_remaining(tdev);
    remainingPulse = remainingPulse + remainingCycles;
    tdev->targetClock = rclock + remainingPulse;
  } else {
    //tdev->remainingCycles = 120000000;
  }

  interrupt_flag();
}

void Java_com_johan_emulator_engine_Emu6502_togglePlay() {
  playDown = !playDown;
}

int isPlayDownBit() {
  return (playDown != 0) ? 0 : 1;
}

void setMotorOn(struct timer_struct *tdev, int motorBit) {
  int oldStarted = tdev->started;
  tdev -> started = (motorBit == 0) ? 1 : 0;

  if (oldStarted != tdev->started) {
    if (tdev->started) {
      tdev->targetClock = remainingPulse + getTotalCycleCount();
    } else {
      remainingPulse = tdev->targetClock - getTotalCycleCount();
    }
  }
}

int getMotorOnBit(struct timer_struct *tdev) {
  return (tdev -> started == 1) ? 0 : 1;
}

struct timer_struct getTapeInstance() {
  //get tape instance on mem init to add to timer list
  //howver, should initially be stopped
  //when new tape attached
  //   -> in front activity create tape buffer image
  //   -> pass down to native wrapper in emu6502
  //   -> store refer in this class
  // in memory trigger start/stop
  //when start/stop trigger methods in this class
  // -> schedule count down + interrupt
  //when attached load remaining
  struct timer_struct mytape;
  mytape.expiredevent = &tape_pulse_expired;
  mytape.targetClock = 0x0;
  mytape.started = 0;
  mytape.interrupt = &interrupt_flag;
  return mytape;
}
