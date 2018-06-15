void red() {
  noInterrupts();
  for (int i = 0; i < 8; i++) {
    write0();
  }
  for (int i = 0; i < 8; i++) {
    write1();
  }
  for (int i = 0; i < 8; i++) {
    write0();
  }
  color_led = 1;
  interrupts();
}

void green() {
  noInterrupts();
  for (int i = 0; i < 8; i++) {
    write1();
  }
  for (int i = 0; i < 8; i++) {
    write0();
  }
  for (int i = 0; i < 8; i++) {
    write0();
  }
  color_led = 2;
  interrupts();
}

void blue() {
  noInterrupts();
  for (int i = 0; i < 8; i++) {
    write0();
  }
  for (int i = 0; i < 8; i++) {
    write0();
  }
  for (int i = 0; i < 8; i++) {
    write1();
  }
  color_led = 3;
  interrupts();
}

void black() {
  noInterrupts();
  for (int i = 0; i < 8; i++) {
    write0();
  }
  for (int i = 0; i < 8; i++) {
    write0();
  }
  for (int i = 0; i < 8; i++) {
    write0();
  }
  color_led = 0;
  interrupts();
}

void write1() {
  for (int i = 0; i < 5; i++) {
    GPIOA->regs->ODR |= 0b0001000000000001; //1
  }
  for (int i = 0; i < 2; i++) {
    GPIOA->regs->ODR &= ~0b0001000000000001; //0
  }
}

void write0() {
  GPIOA->regs->ODR |= 0b0001000000000001; //1
  GPIOA->regs->ODR |= 0b0001000000000001; //1
  GPIOA->regs->ODR |= 0b0001000000000001; //1
  GPIOA->regs->ODR &= ~0b0001000000000001; //0
  GPIOA->regs->ODR &= ~0b0001000000000001; //0
  for (int i = 0; i < 3; i++) {
    millis();
  }
}
