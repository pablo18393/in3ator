
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

void initSensors() {
  if (DHTSensor) {
    dht.setup(DHTPIN);
  }
  if (BME280Sensor) {
    bme.begin();
  }
  A_set = false;
  B_set = false;
  pinMode(encoderpinA, INPUT_PULLUP);
  pinMode(encoderpinB, INPUT_PULLUP);
  attachInterrupt(ENC_A, encoderISR, CHANGE);
  attachInterrupt(ENC_B, encoderISR, CHANGE);
  // timer setup for encoder
  initPulsioximeterVariables();
}

void initTFT() {
  SPI.setModule(SPI_SEL);
  tft.begin();
  tft.setRotation(1);
  //loadLogo();
}

void pinDirection() {
  pinMode(SCREENBACKLIGHT, OUTPUT);
  pinMode(ENC_SWITCH, INPUT_PULLUP);
  pinMode(JAUNDICE, OUTPUT);
  pinMode(POWER_EN, OUTPUT);
  pinMode(FAN_HP, OUTPUT);
  pinMode(FAN_LP, OUTPUT);
  pinMode(STERILIZE, OUTPUT);
  pinMode(HUMIDIFIER, OUTPUT);
  pinMode(HEATER, OUTPUT);
  pinMode(HEATER, OUTPUT);
  pinMode(GSM_PWRKEY, OUTPUT);

  digitalWrite(SCREENBACKLIGHT, HIGH);
  digitalWrite(JAUNDICE, LOW);
  digitalWrite(HEATER, LOW);
  digitalWrite(POWER_EN, HIGH);
  digitalWrite(FAN_HP, LOW);
  digitalWrite(FAN_LP, LOW);
  digitalWrite(STERILIZE, LOW);
  digitalWrite(HUMIDIFIER, LOW);
  digitalWrite(GSM_PWRKEY, HIGH);
}


void initTimers() {
  // PID setup

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
  sensorsTimer.refresh();
  sensorsTimer.resume();

  GSMTimer.pause();
  GSMTimer.setPeriod(GSMISRRate); // in microseconds
  GSMTimer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  GSMTimer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
  GSMTimer.attachCompare1Interrupt(GSMHandler);
  nvic_irq_set_priority(NVIC_TIMER1_CC, 1);
  nvic_irq_set_priority(NVIC_USART1 , 0);
  GSMTimer.refresh();
  GSMTimer.resume();
}
