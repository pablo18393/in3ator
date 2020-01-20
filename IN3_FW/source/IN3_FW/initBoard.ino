#define testMode false
#define operativeMode true

void initBoard() {
  initEEPROM();
  initSensors();
  initSD();
  initTimers();
  pinDirection(testMode);
  initTFT();
  //hardwareVerification();
  pinDirection(operativeMode);
  pwmWrite(SCREENBACKLIGHT, TFT_LED_PWR);
  //digitalWrite(POWER_EN, HIGH);
  initGPRS();
  /*
    if (!firstTurnOn) {
    factoryMenu();
    }
  */
}

void initTFT() {
  SPI.setModule(SPI_SEL);
  tft.begin();
  tft.setRotation(1);
  loadlogo();
}

void pinDirection(bool mode) {
  switch (mode) {
    case testMode:
      afio_cfg_debug_ports(AFIO_DEBUG_SW_ONLY); //release PA15 pin for GPIO purpose
      if (SerialDebug) {
        Serial.begin(115200);
      }
      else {
        Serial.end();
      }
      pinMode(HUMIDIFIER, PWM);
      pinMode(SCREENBACKLIGHT, PWM);
      pinMode(JAUNDICE, OUTPUT);
      pinMode(HEATER, OUTPUT);
      pinMode(FAN_HP, OUTPUT);
      pinMode(BUZZER, OUTPUT);
      pinMode(FAN_LP, OUTPUT);
      pinMode(STERILIZE_CTL, OUTPUT);
      pinMode(ENC_SWITCH, INPUT_PULLUP);
      pinMode(POWER_EN, OUTPUT);
      pinMode(GPRS_PWRKEY, OUTPUT);
      pinMode(encoderpinA, INPUT_PULLUP);
      pinMode(encoderpinB, INPUT_PULLUP);
      pinMode(SYSTEM_SHUNT, INPUT);
      pwmWrite(HUMIDIFIER, 0);
      pwmWrite(SCREENBACKLIGHT, screenBackLightMaxPWM);
      digitalWrite(POWER_EN, LOW);
      digitalWrite(JAUNDICE, LOW);
      digitalWrite(HEATER, LOW);
      digitalWrite(FAN_HP, LOW);
      digitalWrite(BUZZER, LOW);
      digitalWrite(FAN_LP, LOW);
      digitalWrite(STERILIZE_CTL, LOW);
      digitalWrite(GPRS_PWRKEY, HIGH);
      break;

    case operativeMode:
      afio_cfg_debug_ports(AFIO_DEBUG_SW_ONLY); //release PA15 pin for GPIO purpose
      if (SerialDebug) {
        Serial.begin(115200);
      }
      else {
        Serial.end();
      }
      pinMode(JAUNDICE, PWM);
      pinMode(HEATER, PWM);
      pinMode(FAN_HP, PWM);
      pinMode(HUMIDIFIER, PWM);
      pinMode(BUZZER, PWM);
      pinMode(SCREENBACKLIGHT, PWM);
      pinMode(FAN_LP, OUTPUT);
      pinMode(STERILIZE_CTL, OUTPUT);
      pinMode(ENC_SWITCH, INPUT_PULLUP);
      pinMode(POWER_EN, OUTPUT);
      pinMode(GPRS_PWRKEY, OUTPUT);
      pinMode(encoderpinA, INPUT_PULLUP);
      pinMode(encoderpinB, INPUT_PULLUP);
      pinMode(SYSTEM_SHUNT, INPUT);
      pwmWrite(SCREENBACKLIGHT, screenBackLightMaxPWM);
      pwmWrite(JAUNDICE, 0);
      pwmWrite(HEATER, 0);
      pwmWrite(FAN_HP, 0);
      pwmWrite(HUMIDIFIER, 0);
      pwmWrite(BUZZER, 0);
      digitalWrite(FAN_LP, LOW);
      digitalWrite(STERILIZE_CTL, LOW);
      digitalWrite(GPRS_PWRKEY, HIGH);
      break;
  }
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
  encoderTimer.refresh();
  encoderTimer.resume();


  //sensors handling ISR configuration
  sensorsTimer.pause();
  sensorsTimer.setPeriod(sensorsISRRate); // in microseconds
  sensorsTimer.refresh();
  sensorsTimer.resume();

  //GPRS timer configuration
  GPRSTimer.pause();
  GPRSTimer.setPeriod(GPRSISRRate); // in microseconds
  GPRSTimer.refresh();
  GPRSTimer.resume();

  heaterMaxPWM = humidifierTimer.getOverflow();
  jaundiceMaxPWM = humidifierTimer.getOverflow();
  humidifierMaxPWM = humidifierTimer.getOverflow();
  fanHPMaxPWM = GPRSTimer.getOverflow();
  buzzerMaxPWM = sensorsTimer.getOverflow();
  screenBackLightMaxPWM = sensorsTimer.getOverflow();
}
