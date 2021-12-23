#define testMode false
#define operativeMode true

#define testTime 200

#define NTC_BABY_MIN 0
#define NTC_BABY_MAX 60
#define DIG_TEMP_ROOM_MIN 0
#define DIG_TEMP_ROOM_MAX 60
#define DIG_HUM_ROOM_MIN 1
#define DIG_HUM_ROOM_MAX 100

#define HEATER_CONSUMPTION_MIN 1
#define FAN_CONSUMPTION_MIN 0.05
#define PHOTOTHERAPY_CONSUMPTION_MIN 0.07
#define HUMIDIFIER_CONSUMPTION_MIN 0.07

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

#define HW_TEST_OVERRIDE true
#define TCACheckPeriod 1000
#define TFTCheckPeriod 1000
long HW_error = false;
long lastTCACheck, lastTFTCheck;
bool GPIOexpanderPinToCheck[16] = {1, 1, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0};
bool GPIOexpanderStatus[16];
long lastTCAWrite[16];
long lastBugSimulation = false;

#define POWER_EN GPIO_EXP_0
#define GPRS_EN GPIO_EXP_1
#define SD_CS GPIO_EXP_2
#define FAN GPIO_EXP_3
#define HUMIDIFIER GPIO_EXP_6
#define PHOTOTHERAPY GPIO_EXP_7
#define GPRS_PWRKEY GPIO_EXP_8
#define TFT_CS GPIO_EXP_9
#define TOUCH_IRQ GPIO_EXP_10
#define TOUCH_CS GPIO_EXP_11
#define TFT_RST GPIO_EXP_13

void initHardware() {
  logln("[HW] -> Initialiting hardware");
  initI2CBus();
  initGPIOExpander();
  initCurrentSensor();
  initPin(PHOTOTHERAPY, OUTPUT);
  GPIOWrite(PHOTOTHERAPY, HIGH);
  initSensors();
  initSenseCircuit();
  initPWMGPIO();
  initTFT();
  initPowerAlarm();
  initBuzzer();
  initInterrupts();
  initPowerEn();
  initActuators();
  initGPRS();
  if (WIFI_EN) {
    wifiInit();
  }
  if (!HW_error) {
    logln("[HW] -> HARDWARE OK");
    GPRSSetPostVariables(NULL, "HW OK");
  }
  else {
    logln("[HW] -> HARDWARE TEST FAIL");
    logln("[HW] -> HARDWARE ERROR CODE:" + String(HW_error, HEX));
    GPRSSetPostVariables(NULL, "HW FAIL" + String(HW_error, HEX));
    if (!HW_TEST_OVERRIDE) {
      drawHardwareErrorMessage(HW_error);
      while (GPIORead(ENC_SWITCH)) {
        updateData();
      }
    }
  }
  buzzerTone(2, buzzerStandbyToneDuration, buzzerStandbyTone);
}

void initI2CBus() {
  Wire.begin();
}

void initCurrentSensor() {
  if (DIGITAL_CURRENT_SENSOR) {
    PAC.begin();
    PAC.UpdateCurrent();
  }
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
  logln("[HW] -> Initialiting TCA9355");
  TCA.begin();
  for (int pin = 0; pin < 16; pin++)
  {
    TCA.setPolarity(pin, false);
    GPIOexpanderStatus[pin] = false;
  }
  initPin(UNUSED_GPIO_EXP0, OUTPUT);
  initPin(UNUSED_GPIO_EXP1, OUTPUT);
  initPin(UNUSED_GPIO_EXP2, OUTPUT);
  initPin(UNUSED_GPIO_EXP3, OUTPUT);
  GPIOWrite(UNUSED_GPIO_EXP0, HIGH);
  GPIOWrite(UNUSED_GPIO_EXP1, HIGH);
  GPIOWrite(UNUSED_GPIO_EXP2, HIGH);
  GPIOWrite(UNUSED_GPIO_EXP3, HIGH);
}

void initPin(uint8_t GPIO, uint8_t Mode) {
  if (GPIO < GPIO_EXP_BASE) {
    pinMode(GPIO, Mode);
  }
  else {
    TCA.pinMode(GPIO - GPIO_EXP_BASE, Mode);
  }
}

void initPowerAlarm() {

}

bool checkTCAHealth() {
  bool restart = false;
  if (millis() - lastTCACheck > TCACheckPeriod) {
    for (int pin = 0; pin < 16; pin++)
    {
      if (GPIOexpanderPinToCheck[pin]) {
        int val = TCA.digitalRead(pin);
        Serial.println("Checking EXT GPIO: " + String(pin) + "->" + String (val) + String(GPIOexpanderStatus[pin]) + ", polarity: " + String(TCA.getPolarity(pin)));
        if (GPIOexpanderStatus[pin] != val) {
          logln("[HW] -> TCA9355 pin " + String(pin) +  " CHANGED RANDOMLY FROM " + String(GPIOexpanderStatus[pin]) + " TO " + String(val));
          restart = true;
        }
        delay(5);
      }
      lastTCACheck = millis();
    }
    if (restart) {
      TCARestore();
    }
    if (millis() - lastBugSimulation > 10 * TCACheckPeriod) {
      logln("[DEBUG] -> FAKING TCA ERROR");
      // read diagnostics (optional but can help debug problems)
      TCA.digitalWrite(7, HIGH);
      TCA.digitalWrite(9, HIGH);
      lastBugSimulation = millis();
    }
  }
}

void checkTFTHealth() {
  if (millis() - lastTFTCheck > TFTCheckPeriod) {
    tft.setRotation(3);
    uint8_t powerMode = tft.readcommand8(ILI9341_RDMODE);
    uint8_t MADCTL = tft.readcommand8(ILI9341_RDMADCTL);
    uint8_t pixelFormat = tft.readcommand8(ILI9341_RDPIXFMT);
    uint8_t imageFormat = tft.readcommand8(ILI9341_RDIMGFMT);
    uint8_t selfDiag = tft.readcommand8(ILI9341_RDSELFDIAG);
    logln("[HW] -> TFT display status is: " + String(powerMode, HEX) + String(MADCTL, HEX) + String(pixelFormat, HEX) + String(imageFormat, HEX) + String(selfDiag, HEX));
    if (!powerMode && !MADCTL && !pixelFormat && !imageFormat && !selfDiag) {
      TFTRestore();
    }
    //lastTFTCheck=millis();
  }
}

void TCARestore() {
  logln("[HW] -> restoring TCA9355 previous status");
  while (!TCA.begin());

  initPin(POWER_EN, OUTPUT);
  initPin(GPRS_EN, OUTPUT);
  initPin(SD_CS, INPUT);
  initPin(FAN, OUTPUT);
  initPin(HUMIDIFIER, OUTPUT);
  initPin(PHOTOTHERAPY, OUTPUT);

  for (int pin = 0; pin < 16; pin++)
  {
    if (GPIOexpanderPinToCheck[pin]) {
      TCA.setPolarity(pin, false);
      GPIOWrite(GPIO_EXP_BASE + pin, GPIOexpanderStatus[pin]);
    }
  }
}

void GPIOWrite(uint8_t GPIO, uint8_t Mode) {
  if (GPIO < GPIO_EXP_BASE) {
    digitalWrite(GPIO, Mode);
  }
  else {
    logln("[HW] -> TCA9355 writing pin" + String(GPIO - GPIO_EXP_BASE) + " -> " + String(Mode));
    GPIOexpanderStatus[GPIO - GPIO_EXP_BASE] = Mode;
    if (!TCA.digitalWrite(GPIO - GPIO_EXP_BASE, Mode)) {
      logln("[HW] -> TCA9355 WRITE ERROR");
    }
    lastTCAWrite[GPIO - GPIO_EXP_BASE] = millis();
    delay(5);
    //TCA.digitalWrite(GPIO - GPIO_EXP_BASE, Mode);
  }
}

bool GPIORead(uint8_t GPIO) {
  if (GPIO < GPIO_EXP_BASE) {
    return (digitalRead(GPIO));
  }
  else {
    return (TCA.digitalRead(GPIO - GPIO_EXP_BASE));
  }
}

void initGPRS()
{
  Serial2.begin(115200);
  GPRS.enable = true;
  GPRS.powerUp = true;
  GPRSSetPostVariables(defaultPost, "First post, FW version: " + String (FWversion));
  setGPRSPostPeriod(standByGPRSPostPeriod);
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

  testCurrent = measureConsumptionForTime(testTime);
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
  offsetCurrent = measureConsumptionForTime(testTime);
  initPin(TOUCH_CS, OUTPUT);
  initPin(SD_CS, OUTPUT);
  initPin(TFT_CS, OUTPUT);
  initPin(TFT_RST, OUTPUT);
  GPIOWrite(TOUCH_CS, HIGH);
  GPIOWrite(SD_CS, HIGH);
  GPIOWrite(TFT_CS, LOW);
  GPIOWrite(TFT_RST, LOW);  // alternating HIGH/LOW
  GPIOWrite(TFT_RST, HIGH);  // alternating HIGH/LOW
  tft.begin();
  tft.setRotation(3);
  loadlogo();
  ledcWrite(SCREENBACKLIGHT_PWM_CHANNEL, maxPWMvalue);
  for (int i = maxPWMvalue; i >= backlightPower; i--) {
    ledcWrite(SCREENBACKLIGHT_PWM_CHANNEL, i);
    delayMicroseconds(brightenRate);
  }
  testCurrent = measureConsumptionForTime(testTime) - offsetCurrent;
  if (testCurrent < SCREEN_CONSUMPTION_MIN) {
    //HW_error += DEFECTIVE_SCREEN;
    logln("[HW] -> WARNING -> Screen current is not high enough");
  }
  if (testCurrent > SCREEN_CONSUMPTION_MAX) {
    //HW_error += DEFECTIVE_SCREEN;
    logln("[HW] -> WARNING -> Screen current exceeded");
  }
  if (error == HW_error) {
    logln("[HW] -> OK -> Screen is working as expected: " + String (testCurrent) + " Amps");
  }
  else {
    logln("[HW] -> Fail -> test current is " + String (testCurrent) + " Amps");
  }
  GPRSSetPostVariables(NULL, ",TFT:" + String (testCurrent));
}

void TFTRestore() {
  logln("[HW] -> restoring TFT previous status");
  initPin(TOUCH_CS, OUTPUT);
  initPin(SD_CS, OUTPUT);
  initPin(TFT_CS, OUTPUT);
  initPin(TFT_RST, OUTPUT);
  initPin(TOUCH_IRQ, INPUT);
  initPin(TOUCH_CS, INPUT);
  GPIOWrite(TOUCH_CS, HIGH);
  GPIOWrite(SD_CS, HIGH);
  GPIOWrite(TFT_CS, LOW);
  GPIOWrite(TFT_RST, LOW);  // alternating HIGH/LOW
  GPIOWrite(TFT_RST, HIGH);  // alternating HIGH/LOW
  tft.begin();
  tft.setRotation(3);
  switch (page) {
    case actuatorsProgressPage:
      actuatorsProgress();
      break;
    case advancedModePage:
    default:
      advancedMode();
      break;
  }
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

  offsetCurrent = measureConsumptionForTime(testTime);
  ledcWrite(BUZZER_PWM_CHANNEL, buzzerMaxPWM / 2);

  testCurrent = measureConsumptionForTime(testTime) - offsetCurrent;
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

  offsetCurrent = measureConsumptionForTime(testTime);
  logln("[HW] -> Checking power enable circuit...");
  initPin(POWER_EN, OUTPUT);
  GPIOWrite(POWER_EN, HIGH);
  initPin(GPRS_EN, OUTPUT);
  GPIOWrite(GPRS_EN, HIGH);

  testCurrent = measureConsumptionForTime(testTime) - offsetCurrent;
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

  offsetCurrent = measureConsumptionForTime(testTime);
  logln("[HW] -> Checking actuators...");

  GPIOWrite(PHOTOTHERAPY, HIGH);
  testCurrent = measureConsumptionForTime(testTime) - offsetCurrent;
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
  testCurrent = measureConsumptionForTime(testTime) - offsetCurrent;
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

  GPIOWrite(FAN, HIGH);
  testCurrent = measureConsumptionForTime(testTime) - offsetCurrent;
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

  ledcWrite(HEATER_PWM_CHANNEL, maxPWMvalue);
  testCurrent = measureConsumptionForTime(testTime) - offsetCurrent;
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

  if (error == HW_error) {
    logln("[HW] -> OK -> Actuators are working as expected");
  }
  else {
    logln("[HW] -> Fail -> Some actuators are not working as expected");
  }
}
