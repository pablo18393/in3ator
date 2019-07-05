void initBoard() {
  Serial.begin(115200);
  Serial.print("IN3ATOR, VERSION ");
  Serial.println(FWversion);
  pinMode(PC11, OUTPUT);
  digitalWrite(PC11, HIGH);
  delay(1000);
  pinMode(PC14, OUTPUT);
  delay(1200);
  digitalWrite(PC14, HIGH);
  delay(1200);
  digitalWrite(PC14, LOW);

  Serial1.begin(115200);
  Serial1.print("HELLO");
  initEEPROM();
  pinDirection();
  initPIDTimers();
  tft.begin(); 
  tft.setRotation(1);
  //loadLogo();
  dht.setup(DHTPIN);
  initSensors();
}

void initSensors() {
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
  sensorsTimer.pause();
  sensorsTimer.setPeriod(sensorsRate); // in microseconds
  sensorsTimer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  sensorsTimer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
  sensorsTimer.attachCompare1Interrupt(sensorsISR);
  sensorsTimer.refresh();
  sensorsTimer.resume();
}

void pinDirection() {
  pinMode(SCREENBACKLIGHT, OUTPUT);
  pinMode(ENC_PULSE, INPUT_PULLUP);
  pinMode(JAUNDICE, OUTPUT);
  pinMode(HEATER, OUTPUT);
  pinMode(POWER_EN, OUTPUT);
  pinMode(FAN_HP, OUTPUT);
  pinMode(FAN_LP, OUTPUT);
  pinMode(STERILIZE, OUTPUT);
  pinMode(HUMIDIFIER, OUTPUT);

  digitalWrite(SCREENBACKLIGHT, HIGH);
  digitalWrite(JAUNDICE, LOW);
  digitalWrite(HEATER, LOW);
  digitalWrite(POWER_EN, LOW);
  digitalWrite(FAN_HP, LOW);
  digitalWrite(FAN_LP, LOW);
  digitalWrite(STERILIZE, LOW);
  digitalWrite(HUMIDIFIER, LOW);
}
