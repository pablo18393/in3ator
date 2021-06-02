#define testMode false
#define operativeMode true

#define NTC_BABY_MIN 0
#define NTC_BABY_MAX 60
#define DIG_TEMP_ROOM_MIN 0
#define DIG_TEMP_ROOM_MAX 60
#define DIG_HUM_ROOM_MIN 1
#define DIG_HUM_ROOM_MAX 100

#define HEATER_CONSUMPTION_MIN 2
#define FAN_CONSUMPTION_MIN 0.05
#define PHOTOTHERAPY_CONSUMPTION_MIN 0.2
#define HUMIDIFIER_CONSUMPTION_MIN 0.05

#define HEATER_CONSUMPTION_MAX 9
#define FAN_CONSUMPTION_MAX 2
#define PHOTOTHERAPY_CONSUMPTION_MAX 4
#define HUMIDIFIER_CONSUMPTION_MAX 1

#define STANDBY_CONSUMPTION_MIN 0.01
#define STANDBY_CONSUMPTION_MAX 1

#define SCREEN_CONSUMPTION_MIN 0.02
#define SCREEN_CONSUMPTION_MAX 1

#define BUZZER_CONSUMPTION_MIN 0.003

#define NTC_BABY_MIN_ERROR 1
#define NTC_BABY_MAX_ERROR 1<<1
#define DIG_TEMP_ROOM_MIN_ERROR 1<<2
#define DIG_TEMP_ROOM_MAX_ERROR 1<<3
#define DIG_HUM_ROOM_MIN_ERROR 1<<4
#define DIG_HUM_ROOM_MAX_ERROR 1<<5
#define DIGITAL_SENSOR_NOTFOUND 1<<6

#define HEATER_CONSUMPTION_MIN_ERROR 1<<7
#define FAN_CONSUMPTION_MIN_ERROR    1<<8
#define PHOTOTHERAPY_CONSUMPTION_MIN_ERROR   1<<9
#define HUMIDIFIER_CONSUMPTION_MIN_ERROR  1<<10

#define HEATER_CONSUMPTION_MAX_ERROR 1<<11
#define FAN_CONSUMPTION_MAX_ERROR    1<<12
#define PHOTOTHERAPY_CONSUMPTION_MAX_ERROR   1<<13
#define HUMIDIFIER_CONSUMPTION_MAX_ERROR  1<<14

#define STANDBY_CONSUMPTION_MAX_ERROR 1<<15
#define DEFECTIVE_POWER_EN 1<<16
#define DEFECTIVE_SCREEN 1<<17
#define DEFECTIVE_BUZZER 1<<18
#define DEFECTIVE_CURRENT_SENSOR 1<<19

long HW_error = 0;
int buzzerBeeps, buzzerToneTime;
long buzzerTime, buzzerStandbyTime;
bool buzzerBuzzing;

void initHardware() {
  logln("[HW] -> Initialiting hardware");
  initPins();
  initGPRS();
  checkCurrentSenseCircuit();
  initTFT();
  buzzerTest();
  initSensors();
  initHeater();
  initPowerEn();
  actuatorsTest();
  if (!HW_error) {
    logln("[HW] -> HARDWARE OK");
    GPRSSetPostVariables(NULL, "HW OK");
  }
  else {
    logln("[HW] -> HARDWARE TEST FAIL");
    logln("[HW] -> HARDWARE ERROR CODE:" + String(HW_error, HEX));
    GPRSSetPostVariables(NULL, "HW FAIL" + String(HW_error, HEX));
    drawHardwareErrorMessage(HW_error);
    while (digitalRead(ENC_SWITCH)) {
      updateData();
    }
  }
  buzzerTone(4, buzzerStandbyToneDuration, buzzerStandbyTone);
}

void initPins() {
  logln("[HW] -> Initialiting pins");
  pinMode(PHOTOTHERAPY, OUTPUT);
  pinMode(FAN, OUTPUT);
  pinMode(HUMIDIFIER, OUTPUT);
  pinMode(ENC_SWITCH, INPUT_PULLUP);
  pinMode(POWER_EN, OUTPUT);
  pinMode(GPRS_PWRKEY, OUTPUT);
  pinMode(SCREENBACKLIGHT, OUTPUT);
  pinMode(encoderpinA, INPUT_PULLUP);
  pinMode(encoderpinB, INPUT_PULLUP);
  pinMode(SYSTEM_SHUNT, INPUT);
  pinMode(PWR_ALERT, INPUT);
  pinMode(HEATER, PWM);
  pwmWrite(HEATER, 0);
  pinMode(BUZZER, PWM);
  pwmWrite(BUZZER, 0);
  digitalWrite(SCREENBACKLIGHT, HIGH);
  digitalWrite(GPRS_PWRKEY, HIGH);
}

void initGPRS()
{
  Serial1.begin(115200);
  GPRS.enable = 1;
  GPRS.powerUp = 1;
  GPRSSetPostVariables(defaultPost, "First post, FW version: " + String (FWversion));
  setGPRSPostPeriod(standByGPRSPostPeriod);
}

void checkCurrentSenseCircuit() {
  standByCurrentTest();
  measureOffsetConsumption();
}

void initSensors() {
  long error = HW_error;
  logln("[HW] -> Initialiting sensors");
  initRoomSensor();
  //sensors verification
  while (!measurenumNTC());
  if (temperature[babyNTC] < NTC_BABY_MIN) {
    logln("[HW] -> Fail -> NTC temperature is lower than expected");
    HW_error += NTC_BABY_MIN_ERROR;
  }
  if (temperature[babyNTC] > NTC_BABY_MAX) {
    logln("[HW] -> Fail -> NTC temperature is higher than expected");
    HW_error += NTC_BABY_MAX_ERROR;
  }
  if (updateRoomSensor()) {
    if (temperature[digitalTempSensor] < DIG_TEMP_ROOM_MIN) {
      logln("[HW] -> Fail -> Room temperature is lower than expected");
      HW_error += DIG_TEMP_ROOM_MIN_ERROR;
    }
    if (temperature[digitalTempSensor] > DIG_TEMP_ROOM_MAX) {
      logln("[HW] -> Fail -> Room temperature is higher than expected");
      HW_error += DIG_TEMP_ROOM_MAX_ERROR;
    }
    if (humidity < DIG_HUM_ROOM_MIN) {
      logln("[HW] -> Fail -> Room humidity is lower than expected");
      HW_error += DIG_HUM_ROOM_MIN_ERROR;
    }
    if (humidity > DIG_HUM_ROOM_MAX) {
      logln("[HW] -> Fail -> Room humidity is higher than expected");
      HW_error += DIG_HUM_ROOM_MAX_ERROR;
    }
  }
  else {
    HW_error += DIGITAL_SENSOR_NOTFOUND;
    logln("[HW] -> Fail -> No room sensor found");
  }
  if (error == HW_error) {
    logln("[HW] -> OK -> Sensors are working as expected");
  }
}

void initRoomSensor() {
  logln("[HW] -> Detecting room sensor...");
  Wire.begin();
  Wire.beginTransmission(roomSensorAddress);
  roomSensorPresent = Wire.endTransmission();
  if (roomSensorPresent == 0) {
    logln("[HW] -> Room sensor succesfully found, initializing...");
    mySHTC3.begin(Wire);
  }
  else {
    Wire.end();
  }
}

void standByCurrentTest() {
  long error = HW_error;
  float testCurrent;
  logln("[HW] -> Measuring standby current...");
  testCurrent = sampleConsumption();
  if (testCurrent < STANDBY_CONSUMPTION_MIN) {
    HW_error += DEFECTIVE_CURRENT_SENSOR;
    logln("[HW] -> Fail -> Defective current sensor");
  }
  if (testCurrent > STANDBY_CONSUMPTION_MAX) {
    HW_error += STANDBY_CONSUMPTION_MAX_ERROR;
    logln("[HW] -> Fail -> Maximum stanby current exceeded");
  }
  if (error == HW_error) {
    logln("[HW] -> OK -> Current sensor is working as expected: " + String (testCurrent) + " Amps");
  }
  else {
    logln("[HW] -> Fail -> test current is " + String (testCurrent) + " Amps");
  }
  GPRSSetPostVariables(NULL, ",CT:" + String (testCurrent));
}

void initTFT() {
  long error = HW_error;
  float testCurrent, offsetCurrent;
  int  timeOut = 4000;
  long processTime = millis();
  offsetCurrent = sampleConsumption();
  SPI.setModule(SPI_SEL);
  tft.begin();
  tft.setRotation(3);
  loadlogo();
  /*
    configTFTBacklightTimer(backlightTimerPeriod);
    pwmWrite(SCREENBACKLIGHT, i);
    TFT_LED_PWR = screenBackLightMaxPWM / 2;
    for (int i = screenBackLightMaxPWM; i >= TFT_LED_PWR; i--) {
    pwmWrite(SCREENBACKLIGHT, i);
    delayMicroseconds(brightenRate);
    if (millis() - processTime > timeOut) {
      i = 0;
    }
    }
  */
  digitalWrite(SCREENBACKLIGHT, LOW);
  testCurrent = sampleConsumption() - offsetCurrent;
  if (testCurrent < SCREEN_CONSUMPTION_MIN) {
    HW_error += DEFECTIVE_SCREEN;
    logln("[HW] -> Fail -> Screen current is not high enough");
  }
  if (testCurrent > SCREEN_CONSUMPTION_MAX) {
    HW_error += DEFECTIVE_SCREEN;
    logln("[HW] -> Fail -> Screen current exceeded");
  }
  if (error == HW_error) {
    logln("[HW] -> OK -> Screen is working as expected: " + String (testCurrent) + " Amps");
  }
  else {
    logln("[HW] -> Fail -> test current is " + String (testCurrent) + " Amps");
  }
  GPRSSetPostVariables(NULL, ",TFT:" + String (testCurrent));
}

void initHeater() {
  configHeaterTimer(heaterPeriod);
}

void configHeaterTimer(int freq) {
  //humidifier timer configuration:
  heaterTimer.pause();
  heaterTimer.setPeriod(freq); // in microseconds
  heaterTimer.refresh();
  heaterTimer.resume();
  heaterMaxPWM = heaterTimer.getOverflow();
}

void configTFTBacklightTimer(int freq) { 
  //humidifier timer configuration:
  TFTbacklightTimer.pause();
  TFTbacklightTimer.setPeriod(freq); // in microseconds
  TFTbacklightTimer.refresh();
  TFTbacklightTimer.resume();
  screenBackLightMaxPWM = TFTbacklightTimer.getOverflow();
}

void configBuzzerTimer(int freq) {
  //buzzer timer configuration:
  buzzerTimer.pause();
  buzzerTimer.setPeriod(freq); // in microseconds
  buzzerTimer.refresh();
  buzzerTimer.resume();
  buzzerMaxPWM = buzzerTimer.getOverflow();
}

void buzzerTest() {
  long error = HW_error;
  float testCurrent, offsetCurrent;
  offsetCurrent = sampleConsumption();
  configBuzzerTimer(buzzerStandardPeriod);
  pwmWrite(BUZZER, buzzerMaxPWM / 2);
  testCurrent = sampleConsumption() - offsetCurrent;
  pwmWrite(BUZZER, buzzerMaxPWM / 0);
  if (testCurrent < BUZZER_CONSUMPTION_MIN) {
    HW_error += DEFECTIVE_BUZZER;
    logln("[HW] -> Fail -> Buzzer current is not high enough");
  }
  if (error == HW_error) {
    logln("[HW] -> OK -> Buzzer is working as expected: " + String (testCurrent) + " Amps");
  }
  else {
    logln("[HW] -> Fail -> test current is " + String (testCurrent) + " Amps");
  }
  GPRSSetPostVariables(NULL, ",Buz:" + String (testCurrent));
}

void buzzerISR() {
  if (millis() - buzzerTime > buzzerToneTime && buzzerBeeps) {
    buzzerBeeps -= buzzerBuzzing;
    buzzerBuzzing = !buzzerBuzzing;
    pwmWrite(BUZZER, buzzerMaxPWM / 2 * buzzerBuzzing);
    buzzerTime = millis();
  }
  if (page == actuatorsProgressPage) {
    buzzerStandbyTime = max(lastUserInteraction, buzzerStandbyTime);
    if (millis() - buzzerStandbyTime > buzzerStandbyPeriod) {
      buzzerStandbyTime = millis();
      buzzerTone(buzzerStandbyToneTimes, buzzerStandbyToneDuration, buzzerStandbyTone);
    }
  }
}

void buzzerTone (int beepTimes, int timeDelay, int freq) {
  buzzerBeeps += beepTimes;
  buzzerToneTime = timeDelay;
}

void initPowerEn() {
  long error = HW_error;
  float testCurrent, offsetCurrent;
  offsetCurrent = sampleConsumption();
  logln("[HW] -> Checking power enable circuit...");
  digitalWrite(POWER_EN, HIGH);
  testCurrent = sampleConsumption() - offsetCurrent;
  if (testCurrent > STANDBY_CONSUMPTION_MAX) {
    HW_error += DEFECTIVE_POWER_EN;
    logln("[HW] -> Fail -> Defective power enable circuit");
  }
  if (error == HW_error) {
    logln("[HW] -> OK -> Power enable is working as expected: " + String (testCurrent) + " Amps");
  }
  else {
    logln("[HW] -> Fail -> test current is " + String (testCurrent) + " Amps");
  }
  GPRSSetPostVariables(NULL, ",PWEN:" + String (testCurrent));
}

void encSwitchISR() {
  encPulseDetected = 1;
  buzzerTone(buzzerStandbyToneTimes, buzzerSwitchDuration, buzzerRotaryEncoderTone);
  lastUserInteraction = millis();
}

void encoderISR() {
  int newPos;
  encoder.tick(); // just call tick() to check the state.
  newPos = encoder.getPosition();
  lastUserInteraction = millis();
  if (abs(newPos - last_encoder_move) > 1) {
    EncMove = int(encoder.getDirection());
    last_encoder_move = newPos;
  }
}

void actuatorsTest() {
  long error = HW_error;
  float testCurrent, offsetCurrent;
  offsetCurrent = sampleConsumption();
  logln("[HW] -> Checking actuators...");
  pwmWrite(HEATER, heaterMaxPWM * maxHeaterPower / 100 );
  testCurrent = sampleConsumption() - offsetCurrent;
  logln("[HW] -> Heater current consumption: " + String (testCurrent) + " Amps");
  GPRSSetPostVariables(NULL, ",Hea:" + String (testCurrent));
  if (testCurrent < HEATER_CONSUMPTION_MIN) {
    HW_error += HEATER_CONSUMPTION_MIN_ERROR;
    logln("[HW] -> Fail -> Heater current consumption is too low");
  }
  if (testCurrent > HEATER_CONSUMPTION_MAX) {
    HW_error += HEATER_CONSUMPTION_MAX_ERROR;
    logln("[HW] -> Fail -> Heater current consumption is too high");
  }
  pwmWrite(HEATER, LOW);
  digitalWrite(FAN, HIGH);
  testCurrent = sampleConsumption() - offsetCurrent;
  logln("[HW] -> FAN consumption: " + String (testCurrent) + " Amps");
  GPRSSetPostVariables(NULL, ",Fan:" + String (testCurrent));
  if (testCurrent < FAN_CONSUMPTION_MIN) {
    HW_error += FAN_CONSUMPTION_MIN_ERROR;
    logln("[HW] -> Fail -> Fan current consumption is too low");
  }
  if (testCurrent > FAN_CONSUMPTION_MAX) {
    HW_error += FAN_CONSUMPTION_MAX_ERROR;
    logln("[HW] -> Fail -> Fan current consumption is too high");
  }
  digitalWrite(FAN, LOW);
  digitalWrite(PHOTOTHERAPY, HIGH);
  testCurrent = sampleConsumption() - offsetCurrent;
  logln("[HW] -> Phototherapy current consumption: " + String (testCurrent) + " Amps");
  GPRSSetPostVariables(NULL, ",Pho:" + String (testCurrent));
  if (testCurrent < PHOTOTHERAPY_CONSUMPTION_MIN) {
    HW_error += PHOTOTHERAPY_CONSUMPTION_MIN_ERROR;
    logln("[HW] -> Fail -> PHOTOTHERAPY current consumption is too low");
  }
  if (testCurrent > PHOTOTHERAPY_CONSUMPTION_MAX) {
    HW_error += PHOTOTHERAPY_CONSUMPTION_MAX_ERROR;
    logln("[HW] -> Fail -> PHOTOTHERAPY current consumption is too high");
  }
  digitalWrite(PHOTOTHERAPY, LOW);
  digitalWrite(HUMIDIFIER, HIGH);
  testCurrent = sampleConsumption() - offsetCurrent;
  logln("[HW] -> Humidifier current consumption: " + String (testCurrent) + " Amps");
  GPRSSetPostVariables(NULL, ",Hum:" + String (testCurrent));
  if (testCurrent < HUMIDIFIER_CONSUMPTION_MIN) {
    HW_error += HUMIDIFIER_CONSUMPTION_MIN_ERROR;
    logln("[HW] -> Fail -> HUMIDIFIER current consumption is too low");
  }
  if (testCurrent > HUMIDIFIER_CONSUMPTION_MAX) {
    HW_error += HUMIDIFIER_CONSUMPTION_MAX_ERROR;
    logln("[HW] -> Fail -> HUMIDIFIER current consumption is too high");
  }
  digitalWrite(HUMIDIFIER, LOW);
  if (error == HW_error) {
    logln("[HW] -> OK -> Actuators are working as expected");
  }
  else {
    logln("[HW] -> Fail -> Some actuators are not working as expected");
  }
}
