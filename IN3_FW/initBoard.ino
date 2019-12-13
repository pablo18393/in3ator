
void initBoard() {
  pinDirection();
  //hardwareVerification();
  initEEPROM();
  initSensors();
  initSD();
  initGPRS();
  initTFT();
  initTimers();
}

void initTFT() {
  SPI.setModule(SPI_SEL);
  tft.begin();
  tft.setRotation(1);
  //loadlogo();
}

void pinDirection() {
  pinMode(SCREENBACKLIGHT, OUTPUT);
  pinMode(ENC_SWITCH, INPUT_PULLUP);
  pinMode(JAUNDICE, OUTPUT);
  pinMode(POWER_EN, OUTPUT);
  pinMode(FAN_HP, OUTPUT);
  pinMode(FAN_LP, OUTPUT);
  pinMode(STERILIZE_CTL, OUTPUT);
  pinMode(HUMIDIFIER, OUTPUT);
  pinMode(HEATER, OUTPUT);
  pinMode(HEATER, OUTPUT);
  pinMode(GPRS_PWRKEY, OUTPUT);
  pinMode(encoderpinA, INPUT_PULLUP);
  pinMode(encoderpinB, INPUT_PULLUP);
  pinMode(SYSTEM_SHUNT, INPUT);

  digitalWrite(SCREENBACKLIGHT, HIGH);
  digitalWrite(JAUNDICE, LOW);
  digitalWrite(HEATER, LOW);
  digitalWrite(POWER_EN, HIGH);
  digitalWrite(FAN_HP, LOW);
  digitalWrite(FAN_LP, LOW);
  digitalWrite(STERILIZE_CTL, LOW);
  digitalWrite(HUMIDIFIER, LOW);
  digitalWrite(GPRS_PWRKEY, HIGH);
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
    heaterPID.SetOutputLimits(0, HeatermaxPWM);
    roomPIDTimer.refresh();
    roomPIDTimer.resume();

  
  //sensors handling ISR configuration
  sensorsTimer.pause();
  sensorsTimer.setPeriod(sensorsISRRate); // in microseconds
  sensorsTimer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  sensorsTimer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
  sensorsTimer.attachCompare1Interrupt(sensorsISR);
  nvic_irq_set_priority(NVIC_TIMER8_CC, 14);
  sensorsTimer.refresh();
  sensorsTimer.resume();

  //humidifier timer configuration
  humidifierTimer.pause();
  humidifierTimer.setPeriod(humidifierTimerRate); // in microseconds
  humidifierTimer.refresh();
  humidifierTimer.resume();

    encoderTimer.pause();
  encoderTimer.setPeriod(encoderISRRate); // in microseconds
  encoderTimer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  encoderTimer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
  encoderTimer.attachCompare1Interrupt(encoderISR);
  encoderTimer.refresh();
  encoderTimer.resume();
  */
  //GPRS timer configuration
  GPRSTimer.pause();
  GPRSTimer.setPeriod(GPRSISRRate); // in microseconds
  GPRSTimer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  GPRSTimer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
  GPRSTimer.attachCompare1Interrupt(GPRSHandler);
  nvic_irq_set_priority(NVIC_TIMER1_CC, 15);
  nvic_irq_set_priority(NVIC_USART1 , 13);
  GPRSTimer.refresh();
  GPRSTimer.resume();


}
