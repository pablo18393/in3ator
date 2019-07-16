void initBoard() {
  pinDirection();
  initEEPROM();
  initGPRS();
  initTimers();
  tft.begin();
  tft.setRotation(1);
  //loadLogo();
  initSensors();
}

void initSensors() {
  dht.setup(DHTPIN);
  encodertimer = millis(); // acceleration measurement
  for (byte counter = 0; counter < NUMENCODERS; counter++)
  {
    encstate[counter] = HIGH;
    encflag[counter] = HIGH;
    A_set[counter] = false;
    B_set[counter] = false;
    encoderpos[counter] = 0;
    pinMode(encoderpinA[counter], INPUT_PULLUP);
    pinMode(encoderpinB[counter], INPUT_PULLUP);
    lastEncoderPos[counter] = 1;
  }
  // timer setup for encoder
  initPulsioximeterVariables();
}

void pinDirection() {
  pinMode(SCREENBACKLIGHT, OUTPUT);
  pinMode(ENC_PULSE, INPUT_PULLUP);
  pinMode(JAUNDICE, OUTPUT);
  pinMode(POWER_EN, OUTPUT);
  pinMode(FAN_HP, OUTPUT);
  pinMode(FAN_LP, OUTPUT);
  pinMode(STERILIZE, OUTPUT);
  pinMode(HUMIDIFIER, OUTPUT);
  dac_init(DAC, HEATER);   // Enable DAC channel in heater pin
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
  heaterPID.SetOutputLimits(0, maxDACvalueHeater);
}
