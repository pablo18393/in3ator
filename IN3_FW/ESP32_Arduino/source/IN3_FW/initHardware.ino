#define testMode false
#define operativeMode true

#define NTC_BABY_MIN 0
#define NTC_BABY_MAX 60
#define DIG_TEMP_ROOM_MIN 0
#define DIG_TEMP_ROOM_MAX 60
#define DIG_HUM_ROOM_MIN 1
#define DIG_HUM_ROOM_MAX 100

#define HEATER_CONSUMPTION_MIN 1
#define FAN_CONSUMPTION_MIN 0.03
#define PHOTOTHERAPY_CONSUMPTION_MIN 0.03
#define HUMIDIFIER_CONSUMPTION_MIN 0.03

#define HEATER_CONSUMPTION_MAX 9
#define FAN_CONSUMPTION_MAX 4
#define PHOTOTHERAPY_CONSUMPTION_MAX 5
#define HUMIDIFIER_CONSUMPTION_MAX 1

#define STANDBY_CONSUMPTION_MIN 0
#define STANDBY_CONSUMPTION_MAX 1

#define SCREEN_CONSUMPTION_MIN 0
#define SCREEN_CONSUMPTION_MAX 1

#define BUZZER_CONSUMPTION_MIN 0

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

long HW_error = false;

void initHardware() {
  logln("[HW] -> Initialiting hardware");
  initI2CBus();
  initGPIOExpander();
  initCurrentSensor();
  initPWMGPIO();
  initPowerAlarm();
  initGPRS();
  initSenseCircuit();
  initTFT();
  initBuzzer();
  initSensors();
  initPowerEn();
  initActuators();
  if (!HW_error) {
    logln("[HW] -> HARDWARE OK");
    GPRSSetPostVariables(NULL, "HW OK");
  }
  else {
    logln("[HW] -> HARDWARE TEST FAIL");
    logln("[HW] -> HARDWARE ERROR CODE:" + String(HW_error, HEX));
    GPRSSetPostVariables(NULL, "HW FAIL" + String(HW_error, HEX));
    drawHardwareErrorMessage(HW_error);
    while (GPIORead(ENC_SWITCH)) {
      updateData();
    }
  }
  buzzerTone(4, buzzerStandbyToneDuration, buzzerStandbyTone);
}

void initI2CBus() {
  Wire.begin();
}

void initCurrentSensor() {
  PAC.begin();
  PAC.UpdateCurrent();
}

void initPWMGPIO() {
  ledcSetup(SCREENBACKLIGHT_PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcSetup(HEATER_PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);
  ledcSetup(BUZZER_PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);

  ledcAttachPin(SCREENBACKLIGHT, SCREENBACKLIGHT_PWM_CHANNEL);
  ledcAttachPin(HEATER, HEATER_PWM_CHANNEL);
  ledcAttachPin(BUZZER, BUZZER_PWM_CHANNEL);

  ledcWrite(SCREENBACKLIGHT_PWM_CHANNEL, false);
  ledcWrite(HEATER_PWM_CHANNEL, false);
  ledcWrite(BUZZER_PWM_CHANNEL, false);
}

void initGPIOExpander() {
  TCA.begin();
}

void initPin(uint8_t GPIO, uint8_t Mode) {
  if (GPIO < MUX_BASE) {
    pinMode(GPIO, Mode);
  }
  else {
    TCA.pinMode(GPIO - MUX_BASE, Mode);
  }
}

void initPowerAlarm() {
  pinMode(PWR_ALERT, INPUT);
}

void GPIOWrite(uint8_t GPIO, uint8_t Mode) {
  if (GPIO < MUX_BASE) {
    digitalWrite(GPIO, Mode);
  }
  else {
    TCA.digitalWrite(GPIO - MUX_BASE, Mode);
  }
}

bool GPIORead(uint8_t GPIO) {
  if (GPIO < MUX_BASE) {
    return (digitalRead(GPIO));
  }
  else {
    return (TCA.digitalRead(GPIO - MUX_BASE));
  }
}

void initGPRS()
{
  Serial2.begin(115200);
  GPRS.enable = true;
  GPRS.powerUp = true;
  GPRSSetPostVariables(defaultPost, "First post, FW version: " + String (FWversion));
  setGPRSPostPeriod(standByGPRSPostPeriod);
  initPin(GPRS_EN, OUTPUT);
  GPIOWrite(GPRS_EN, HIGH);
  initPin(GPRS_PWRKEY, OUTPUT);
  GPIOWrite(GPRS_PWRKEY, HIGH);
}

void initSenseCircuit() {
  initPin(SYSTEM_SHUNT, INPUT);
  standByCurrentTest();
  measureOffsetConsumption();
}

void initSensors() {
  long error = HW_error;
  logln("[HW] -> Initialiting sensors");
  initRoomSensor();
  initEncoder();
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

void initEncoder() {
  initPin(encoderpinA, INPUT_PULLUP);
  initPin(encoderpinB, INPUT_PULLUP);
  initPin(ENC_SWITCH, INPUT_PULLUP);
}

void initRoomSensor() {
  logln("[HW] -> Detecting room sensor...");
  Wire.beginTransmission(roomSensorAddress);
  roomSensorPresent = !(Wire.endTransmission());
  if (roomSensorPresent == true) {
    logln("[HW] -> Room sensor succesfully found, initializing...");
    mySHTC3.begin(Wire);
  }
}

void standByCurrentTest() {
  long error = HW_error;
  float testCurrent;
  logln("[HW] -> Measuring standby current...");
  testCurrent = measureConsumption();
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
  backlightPower = maxPWMvalue / screenBrightnessFactor;
  backlightPowerSafe = maxPWMvalue * backlightPowerSafePercentage;
  offsetCurrent = measureConsumption();
  initPin(TOUCH_CS, OUTPUT);
  initPin(SD_CS, OUTPUT);
  initPin(TFT_CS, OUTPUT);
  initPin(TFT_RST, OUTPUT);
  GPIOWrite(TOUCH_CS, HIGH);
  GPIOWrite(SD_CS, HIGH);
  GPIOWrite(TFT_CS, LOW);
  GPIOWrite(TFT_RST, LOW);  // alternating HIGH/LOW
  delay(10);
  GPIOWrite(TFT_RST, HIGH);  // alternating HIGH/LOW
  tft.begin();
  tft.setRotation(3);
  loadlogo();
  ledcWrite(SCREENBACKLIGHT_PWM_CHANNEL, maxPWMvalue);
  for (int i = maxPWMvalue; i >= backlightPower; i--) {
    ledcWrite(SCREENBACKLIGHT_PWM_CHANNEL, i);
    delayMicroseconds(brightenRate);
    updateData();
  }
  testCurrent = measureConsumption() - offsetCurrent;
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

void initActuators() {
  initHeater();
  initPin(PHOTOTHERAPY, OUTPUT);
  initPin(FAN, OUTPUT);
  initPin(HUMIDIFIER, OUTPUT);
  actuatorsTest();
}

void initHeater() {

}

void configheaterTimer(int freq) {

}

void configbuzzerTimer(int freq) {

}

void initBuzzer() {
  long error = HW_error;
  float testCurrent, offsetCurrent;
  offsetCurrent = measureConsumption();

  ledcWrite(BUZZER_PWM_CHANNEL, buzzerMaxPWM / 2);
  testCurrent = measureConsumption() - offsetCurrent;
  ledcWrite(BUZZER_PWM_CHANNEL, false);
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
    ledcWrite(BUZZER_PWM_CHANNEL, buzzerMaxPWM / 2 * buzzerBuzzing);
    buzzerTime = millis();
  }
  if (page == actuatorsProgressPage) {
    buzzerStandbyTime = max(lastUserInteraction, buzzerStandbyTime);
  }
}

void buzzerConstantTone (int freq) {
  configbuzzerTimer(freq);
  ledcWrite(BUZZER_PWM_CHANNEL, buzzerMaxPWM / 2);
}

void shutBuzzer () {
  ledcWrite(BUZZER_PWM_CHANNEL, false);
}

void buzzerTone (int beepTimes, int timeDelay, int freq) {
  configbuzzerTimer(freq);
  buzzerBeeps += beepTimes;
  buzzerToneTime = timeDelay;
}

void initPowerEn() {
  long error = HW_error;
  float testCurrent, offsetCurrent;
  offsetCurrent = measureConsumption();
  logln("[HW] -> Checking power enable circuit...");
  initPin(POWER_EN, OUTPUT);
  GPIOWrite(POWER_EN, HIGH);
  testCurrent = measureConsumption() - offsetCurrent;
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

void actuatorsTest() {
  long error = HW_error;
  float testCurrent, offsetCurrent;
  offsetCurrent = measureConsumption();
  logln("[HW] -> Checking actuators...");
  ledcWrite(HEATER_PWM_CHANNEL, heaterMaxPWM * HeaterPower / 100 );
  testCurrent = measureConsumption() - offsetCurrent;
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
  ledcWrite(HEATER_PWM_CHANNEL, 0);
  GPIOWrite(FAN, HIGH);
  testCurrent = measureConsumption() - offsetCurrent;
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
  GPIOWrite(FAN, LOW);
  GPIOWrite(PHOTOTHERAPY, HIGH);
  testCurrent = measureConsumption() - offsetCurrent;
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
  GPIOWrite(PHOTOTHERAPY, LOW);
  GPIOWrite(HUMIDIFIER, HIGH);
  testCurrent = measureConsumption() - offsetCurrent;
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
  GPIOWrite(HUMIDIFIER, LOW);
  if (error == HW_error) {
    logln("[HW] -> OK -> Actuators are working as expected");
  }
  else {
    logln("[HW] -> Fail -> Some actuators are not working as expected");
  }
}
