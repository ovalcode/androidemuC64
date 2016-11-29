//
// Created by johan on 2016/11/29.
//

void tape_pulse_expired(struct timer_struct *tdev) {

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
  struct timer_struct mytape;
  mytape.expiredevent = &tape_pulse_expired;
  mytape.remainingCycles = 0xffff;
  mytape.started = 0;
  mytape.interrupt = &interrupt_timer_A;
  return mytape;
}
