void initBoard() {
  watchdogInit();
  initDebug();
  initEEPROM();
  initHardware();
  //initSD();
  initInterrupts();
  initTimers();
}

void initDebug() {
  Serial4.begin(115200);
  logln("in3ator debug uart, version " + String (FWversion) + ", SN: " + String (serialNumber));
}

void initInterrupts() {
  attachInterrupt(PWR_ALERT, powerAlert, RISING);
}

void initTimers() {
  /*
    roomPIDTimer.pause();
    roomPIDTimer.setPeriod(NTCInterruptRate); // in microseconds
    roomPIDTimer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
    roomPIDTimer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
    roomPIDTimer.attachCompare1Interrupt(roomPIDInterrupt);
    roomPID.SetSampleTime(roomPIDRate / 1000);       //in milliseconds
    heaterPID.SetSampleTime(heaterPIDRate / 1000); //in milliseconds
    heaterPID.SetOutputLimits(0, heaterMaxPWM);
    roomPIDTimer.refresh();
    roomPIDTimer.resume();
  */

  //humidifier timer configuration:
  humidifierTimer.pause();
  humidifierTimer.setPeriod(humidifierTimerRate); // in microseconds
  humidifierTimer.refresh();
  humidifierTimer.resume();

  encoderTimer.pause();
  encoderTimer.setPeriod(peripheralsISRRate); // in microseconds
  encoderTimer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  encoderTimer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
  encoderTimer.attachCompare1Interrupt(peripheralsISR);
  nvic_irq_set_priority(NVIC_TIMER8_CC, 15);
  nvic_irq_set_priority(NVIC_USART1 , 13);
  nvic_irq_set_priority(NVIC_UART4 , 14);
  encoderTimer.refresh();
  encoderTimer.resume();


  //sensors handling ISR configuration
  /*
    sensorsTimer.pause();
    sensorsTimer.setPeriod(sensorsISRRate); // in microseconds
    sensorsTimer.refresh();
    sensorsTimer.resume();
  */
  //GPRS timer configuration
  /*
    GPRSTimer.pause();
    GPRSTimer.setPeriod(GPRSISRRate); // in microseconds
    GPRSTimer.refresh();
    GPRSTimer.resume();
  */
  humidifierMaxPWM = humidifierTimer.getOverflow();
  //fanHPMaxPWM = GPRSTimer.getOverflow();
  screenBackLightMaxPWM = humidifierTimer.getOverflow();
}
