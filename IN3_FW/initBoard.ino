
void initBoard() {
  pinDirection();
  //hardwareVerification();
  initEEPROM();
  initGPRS();
  initSensors();
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
  encodertimer = millis(); // acceleration measurement
  for (byte counter = 0; counter < NUMENCODERS; counter++)
  {
    encstate[ENC_counter] = HIGH;
    encflag[ENC_counter] = HIGH;
    A_set[ENC_counter] = false;
    B_set[ENC_counter] = false;
    encoderpos[ENC_counter] = 0;
    pinMode(encoderpinA[ENC_counter], INPUT_PULLUP);
    pinMode(encoderpinB[ENC_counter], INPUT_PULLUP);
    lastEncoderPos[ENC_counter] = 1;
  }
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
  digitalWrite(POWER_EN, LOW);
  digitalWrite(FAN_HP, LOW);
  digitalWrite(FAN_LP, LOW);
  digitalWrite(STERILIZE, LOW);
  digitalWrite(HUMIDIFIER, LOW);
  digitalWrite(GSM_PWRKEY, LOW);
}


void initTimers() {
  // PID setup for encoder
  roomPIDTimer.pause();
  roomPIDTimer.setPeriod(NTCInterruptRate); // in microseconds
  roomPIDTimer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  roomPIDTimer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
  roomPIDTimer.attachCompare1Interrupt(roomPIDInterrupt);
  roomPIDTimer.refresh();
  roomPIDTimer.resume();

  sensorsTimer.pause();
  sensorsTimer.setPeriod(sensorsRate); // in microseconds
  sensorsTimer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  sensorsTimer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
  sensorsTimer.attachCompare1Interrupt(sensorsISR);
  sensorsTimer.refresh();
  sensorsTimer.resume();

  roomPID.SetSampleTime(roomPIDRate / 1000);       //in milliseconds
  heaterPID.SetSampleTime(heaterPIDRate / 1000); //in milliseconds
  heaterPID.SetOutputLimits(0, HeatermaxPWM);
}
