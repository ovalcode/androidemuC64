int interrupt_mask;
int interrupts_occured;

void set_mask(int new_mask) {
  if ((new _mask & 0x80) == 0x80) { //set the following masks
    interrupt_mask = (interrupt_mask | new_mask) & 0x7f;
  } else { //clear the following masks
    new_mask = ~new_mask;
    interrupt_mask = interrupt_mask & new_mask;
    interrupt_mask = interrupt_mask & 0x7f;
  }
}

int read_interrupts_register() {
  int ir_msb = ((interrupts_occured & interrupt_mask) != 0) ? 0x80 : 0;
  return ir_msb | interrupts_occured;
}

int trigger_irq() {
  return (interrupts_occured & interrupt_mask);
}

void interrupt_timer_A() {
  interrupts_occured = interrupts_occured | 1;
}

void interrupt_timer_B() {
  interrupts_occured = interrupts_occured | 2;
}

