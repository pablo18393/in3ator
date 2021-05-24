#define testMode false
#define operativeMode true

void initBoard() {
  initDebug();
  initEEPROM();
  initSensors();
  //initSD();
  initTimers();
  pinDirection(testMode);
  //hardwareVerification();
  pinDirection(operativeMode);
  initTFT();
  initGPRS();
  watchdogInit();
}

void initTFT() {
  SPI.setModule(SPI_SEL);
  tft.begin();
  tft.setRotation(3);
  loadlogo();
}

void pinDirection(bool mode) {
  switch (mode) {
    case testMode:
      pinMode(HUMIDIFIER, PWM);
      pinMode(SCREENBACKLIGHT, OUTPUT);
      pinMode(JAUNDICE, OUTPUT);
      pinMode(HEATER, OUTPUT);
      pinMode(FAN, OUTPUT);
      pinMode(BUZZER, OUTPUT);
      pinMode(ENC_SWITCH, INPUT_PULLUP);
      pinMode(POWER_EN, OUTPUT);
      pinMode(GPRS_PWRKEY, OUTPUT);
      pinMode(encoderpinA, INPUT_PULLUP);
      pinMode(encoderpinB, INPUT_PULLUP);
      pinMode(SYSTEM_SHUNT, INPUT);
      pwmWrite(HUMIDIFIER, 0);
      digitalWrite(SCREENBACKLIGHT, HIGH);
      digitalWrite(POWER_EN, LOW);
      digitalWrite(JAUNDICE, LOW);
      digitalWrite(HEATER, LOW);
      digitalWrite(FAN, LOW);
      digitalWrite(BUZZER, LOW);
      digitalWrite(GPRS_PWRKEY, HIGH);
      break;

    case operativeMode:
      pinMode(HEATER, OUTPUT);
      pinMode(FAN, OUTPUT);
      pinMode(HUMIDIFIER, PWM);
      pinMode(BUZZER, PWM);
      pinMode(SCREENBACKLIGHT, OUTPUT);
      pinMode(ENC_SWITCH, INPUT_PULLUP);
      pinMode(POWER_EN, OUTPUT);
      pinMode(GPRS_PWRKEY, OUTPUT);
      pinMode(encoderpinA, INPUT_PULLUP);
      pinMode(encoderpinB, INPUT_PULLUP);
      pinMode(SYSTEM_SHUNT, INPUT);
      pinMode(JAUNDICE, OUTPUT);
      pinMode(BACKUP, OUTPUT);
      digitalWrite(SCREENBACKLIGHT, HIGH);
      digitalWrite(HEATER, LOW);
      digitalWrite(FAN, LOW);
      pwmWrite(HUMIDIFIER, 0);
      pwmWrite(BUZZER, 0);
      digitalWrite(GPRS_PWRKEY, HIGH);
      digitalWrite(POWER_EN, HIGH);
      break;
  }
}

void initDebug() {
  Serial4.begin(115200);
  Serial4.println("in3ator debug uart, version " + String (FWversion) + ", SN: " + String (serialNumber));
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
  buzzerTimer.pause();
  buzzerTimer.setPeriod(buzzerTimerRate); // in microseconds
  buzzerTimer.refresh();
  buzzerTimer.resume();

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
  buzzerMaxPWM = buzzerTimer.getOverflow();
  screenBackLightMaxPWM = humidifierTimer.getOverflow();
}
