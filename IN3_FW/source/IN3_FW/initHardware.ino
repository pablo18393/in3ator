#define testMode false
#define operativeMode true

#define CURRENT_STABILIZE_TIME 500

#define NTC_BABY_MIN 1
#define NTC_BABY_MAX 60
#define DIG_TEMP_ROOM_MIN 1
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
#define HUMIDIFIER_CONSUMPTION_MAX 0.45

#define STANDBY_CONSUMPTION_MIN 0
#define STANDBY_CONSUMPTION_MAX 1

#define SCREEN_CONSUMPTION_MIN 0.005
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

#define DEFECTIVE_SCREEN 1<<17
#define DEFECTIVE_BUZZER 1<<18
#define DEFECTIVE_CURRENT_SENSOR 1<<19

#define I2C_CLK_SPEED 50 //50k

#define TFTCheckPeriod 10000
long HW_error = false;
long lastTFTCheck;

void initDebug() {
  Serial.begin(115200);
  vTaskDelay(CURRENT_STABILIZE_TIME);
  logln("in3ator debug uart, version " + String (FWversion) + ", SN: " + String (serialNumber));
}

void initHardware (bool printOutputTest) {
  initDebug();
  initEEPROM();
  initGPIO();
  logln("[HW] -> Initialiting hardware");
  initSensors();
  initPWMGPIO();
  initSenseCircuit();
  initTFT();
  initPowerAlarm();
  initBuzzer();
  initInterrupts();
  initActuators();
  PINInit();
  initGPRS();
  if (WIFI_EN) {
    wifiInit();
  }
  if (!HW_error) {
    logln("[HW] -> HARDWARE OK");
    GPRSSetPostVariables(NO_COMMENT, "HW OK");
  }
  else {
    logln("[HW] -> HARDWARE TEST FAIL");
    logln("[HW] -> HARDWARE ERROR CODE:" + String(HW_error, HEX));
    GPRSSetPostVariables(NO_COMMENT, "HW FAIL" + String(HW_error, HEX));
  }
  if (printOutputTest) {
    drawHardwareErrorMessage(HW_error);
    while (GPIORead(ENC_SWITCH)) {
      updateData();
    }
  }
  buzzerTone(2, buzzerStandbyToneDuration, buzzerStandbyTone);
  watchdogInit();
  initAlarms();
}

void initGPIO() {
  //checkHWAlternativePins(HWAlternativePinout);
  initPin(PHOTOTHERAPY, OUTPUT);
  initPin(GPRS_PWRKEY, OUTPUT);
  initPin(encoderpinA, INPUT_PULLUP);
  initPin(encoderpinB, INPUT_PULLUP);
  initPin(ENC_SWITCH, INPUT_PULLUP);
  initPin(TFT_CS, OUTPUT);
  initPin(PHOTOTHERAPY, OUTPUT);
  initPin(FAN, OUTPUT);
  initPin(HEATER, OUTPUT);
  initPin(BUZZER, OUTPUT);
  initPin(SCREENBACKLIGHT, OUTPUT);
  initPin(ACTUATORS_EN, INPUT_PULLUP);
}

void initPWMGPIO() {
  ledcSetup(SCREENBACKLIGHT_PWM_CHANNEL, DEFAULT_PWM_FREQUENCY, DEFAULT_PWM_RESOLUTION);
  ledcSetup(HEATER_PWM_CHANNEL, HEATER_PWM_FREQUENCY, DEFAULT_PWM_RESOLUTION);
  ledcSetup(BUZZER_PWM_CHANNEL, DEFAULT_PWM_FREQUENCY, DEFAULT_PWM_RESOLUTION);
  ledcAttachPin(SCREENBACKLIGHT, SCREENBACKLIGHT_PWM_CHANNEL);
  ledcAttachPin(HEATER, HEATER_PWM_CHANNEL);
  ledcAttachPin(BUZZER, BUZZER_PWM_CHANNEL);
  ledcWrite(SCREENBACKLIGHT_PWM_CHANNEL, PWM_MAX_VALUE);
  ledcWrite(HEATER_PWM_CHANNEL, false);
  ledcWrite(BUZZER_PWM_CHANNEL, false);
}

void initInterrupts() {
  attachInterrupt(ENC_SWITCH, encSwitchHandler, CHANGE);
  attachInterrupt(ENC_A, encoderISR, CHANGE);
  attachInterrupt(ENC_B, encoderISR, CHANGE);
}

float readExternalADC() {
}

void checkHWAlternativePins(int mode) {
  switch (mode) {
    case HUMIDIFIER_ALTERNATIVE:
      HUMIDIFIER = HUMIDIFIER_ALTERNATIVE;
      I2C_SCL = I2C_SCL_ALTERNATIVE;
      break;
    default:
      HUMIDIFIER = HUMIDIFIER_DEFAULT;
      I2C_SCL = I2C_SCL_DEFAULT;
      break;
  }
  I2C_SDA = I2C_SDA_DEFAULT;
  logln("[HW] -> I2C Pins are " + String (I2C_SDA) + "," + String(I2C_SCL));
}

void alternateHWPins() {
  initPin(HUMIDIFIER, INPUT);
  if (HUMIDIFIER == HUMIDIFIER_ALTERNATIVE) {
    HUMIDIFIER = HUMIDIFIER_DEFAULT;
    I2C_SCL = I2C_SCL_DEFAULT;
  }
  else {
    HUMIDIFIER = HUMIDIFIER_ALTERNATIVE;
    I2C_SCL = I2C_SCL_ALTERNATIVE;
  }
  initPin(HUMIDIFIER, OUTPUT);
  GPIOWrite(HUMIDIFIER, LOW);
  EEPROM.write(EEPROM_HWAlternativePinout, HUMIDIFIER);
  EEPROM.commit();
}

void initI2CBus() {
  initI2CBus(I2C_SDA_DEFAULT, I2C_SCL_DEFAULT);
}

void initI2CBus(int I2C_SDA, int I2C_SCL) {
  deninitI2CBus();
  Wire.begin(I2C_SDA, I2C_SCL);
  wire = &Wire;
}

void deninitI2CBus() {
  Wire.end();
}

void initRoomSensor() {
  roomSensorPresent = false;
  wire->beginTransmission(roomSensorAddress);
  roomSensorPresent = !(wire->endTransmission());
  if (roomSensorPresent == true) {
    logln("[HW] -> Room sensor succesfully found, initializing...");
    mySHTC3.begin(Wire);
  }
}

void initCurrentSensor() {
  wire->beginTransmission(digitalCurrentSensor_i2c_address);
  digitalCurrentSensorPresent = !(wire->endTransmission());
  if (digitalCurrentSensorPresent) {
    digitalCurrentSensor.begin();
    digitalCurrentSensor.reset();

    // Set shunt resistors to 10 mOhm for all channels
    digitalCurrentSensor.setShuntRes(SYSTEM_SHUNT, PHOTOTHERAPY_SHUNT, FAN_SHUNT);

    logln("[HW] -> digital sensor detected, current consumption is " + String(digitalCurrentSensor.getCurrent(INA3221_CH1), 2) + " Amperes");
  }
  else {
    logln("[HW] -> no digital sensor detected");
  }
}

void initPin(uint8_t GPIO, uint8_t Mode) {
  pinMode(GPIO, Mode);
}

void initPowerAlarm() {

}

void checkTFTHealth() {
  if (millis() - lastTFTCheck > TFTCheckPeriod) {
    uint8_t powerMode = tft.readcommand8(ILI9341_RDMODE);
    uint8_t MADCTL = tft.readcommand8(ILI9341_RDMADCTL);
    uint8_t pixelFormat = tft.readcommand8(ILI9341_RDPIXFMT);
    uint8_t imageFormat = tft.readcommand8(ILI9341_RDIMGFMT);
    uint8_t selfDiag = tft.readcommand8(ILI9341_RDSELFDIAG);
    logln("[HW] -> TFT display status is: " + String(powerMode, HEX) + String(MADCTL, HEX) + String(pixelFormat, HEX) + String(imageFormat, HEX) + String(selfDiag, HEX));
    if (!powerMode && !MADCTL && !pixelFormat && !imageFormat && !selfDiag) {
      TFTRestore();
    }
    lastTFTCheck = millis();
  }
}

void GPIOWrite(uint8_t GPIO, uint8_t Mode) {
  digitalWrite(GPIO, Mode);
}

bool GPIORead(uint8_t GPIO) {
  return (digitalRead(GPIO));
}

void initSenseCircuit() {
  standByCurrentTest();
}

void initSensors() {
  long error = HW_error;
  logln("[HW] -> Initialiting sensors");
  initI2CBus();
  initCurrentSensor();
  initRoomSensor();
  //sensors verification
  while (!measureNTCTemperature());
  if (temperature[skinSensor] < NTC_BABY_MIN) {
    logln("[HW] -> Fail -> NTC temperature is lower than expected");
    HW_error += NTC_BABY_MIN_ERROR;
  }
  if (temperature[skinSensor] > NTC_BABY_MAX) {
    logln("[HW] -> Fail -> NTC temperature is higher than expected");
    HW_error += NTC_BABY_MAX_ERROR;
  }
  if (updateRoomSensor()) {
    if (temperature[airSensor] < DIG_TEMP_ROOM_MIN) {
      logln("[HW] -> Fail -> Room temperature is lower than expected");
      HW_error += DIG_TEMP_ROOM_MIN_ERROR;
    }
    if (temperature[airSensor] > DIG_TEMP_ROOM_MAX) {
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

void standByCurrentTest() {
  long error = HW_error;
  float testCurrent;
  logln("[HW] -> Measuring standby current...");

  testCurrent = measureMeanConsumption(SYSTEM_SHUNT_CHANNEL);
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
  GPRSSetPostVariables(NO_COMMENT, ",CT:" + String (testCurrent));
}

void initializeTFT() {
  tft.begin(SPI_CLOCK_DIV8);
  tft.setRotation(DISPLAY_DEFAULT_ROTATION);
}

void initTFT() {
  long error = HW_error;
  float testCurrent, offsetCurrent;
  int  timeOut = 4000;
  long processTime;
  if (screenBrightnessFactor) {
    backlightPower = PWM_MAX_VALUE / screenBrightnessFactor;
  }
  backlightPowerSafe = PWM_MAX_VALUE * backlightPowerSafePercentage;
  offsetCurrent = measureMeanConsumption(SYSTEM_SHUNT_CHANNEL);
  GPIOWrite(TFT_CS, LOW);
  initializeTFT();
  loadlogo();
  processTime = millis();
  for (int i = PWM_MAX_VALUE; i >= backlightPower; i--) {
    ledcWrite(SCREENBACKLIGHT_PWM_CHANNEL, i);
    if (brightenRate) {
      while (millis() - processTime < i / brightenRate) {
        sensorsHandler();
      }
    }
    processTime = millis();
  }
  testCurrent = measureMeanConsumption(SYSTEM_SHUNT_CHANNEL) - offsetCurrent;
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
  GPRSSetPostVariables(NO_COMMENT, ",TFT:" + String (testCurrent));
}

void TFTRestore() {
  logln("[HW] -> restoring TFT previous status");
  GPIOWrite(TFT_CS, LOW);
  initializeTFT();
  switch (page) {
    case actuatorsProgressPage:
      UI_actuatorsProgress();
      break;
    case mainMenuPage:
    default:
      UI_mainMenu();
      break;
  }
}

void initActuators() {
  actuatorsTest();
}

void initBuzzer() {
  long error = HW_error;
  float testCurrent, offsetCurrent;

  offsetCurrent = measureMeanConsumption(SYSTEM_SHUNT_CHANNEL);
  ledcWrite(BUZZER_PWM_CHANNEL, BUZZER_MAX_PWM / 2);
  vTaskDelay(CURRENT_STABILIZE_TIME);
  testCurrent = measureMeanConsumption(SYSTEM_SHUNT_CHANNEL) - offsetCurrent;
  ledcWrite(BUZZER_PWM_CHANNEL, false);
  vTaskDelay(CURRENT_STABILIZE_TIME);
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
  GPRSSetPostVariables(NO_COMMENT, ",Buz:" + String (testCurrent));
}

void actuatorsTest() {
  long error = HW_error;
  float testCurrent, offsetCurrent;
  offsetCurrent = measureMeanConsumption(PHOTOTHERAPY_SHUNT_CHANNEL);
  logln("[HW] -> Checking actuators...");
  GPIOWrite(PHOTOTHERAPY, HIGH);
  vTaskDelay(CURRENT_STABILIZE_TIME);
  testCurrent = measureMeanConsumption(PHOTOTHERAPY_SHUNT_CHANNEL) - offsetCurrent;
  GPIOWrite(PHOTOTHERAPY, LOW);
  logln("[HW] -> Phototherapy current consumption: " + String (testCurrent) + " Amps");
  GPRSSetPostVariables(NO_COMMENT, ",Pho:" + String (testCurrent));
  if (testCurrent < PHOTOTHERAPY_CONSUMPTION_MIN) {
    HW_error += PHOTOTHERAPY_CONSUMPTION_MIN_ERROR;
    logln("[HW] -> Fail -> PHOTOTHERAPY current consumption is too low");
  }
  if (testCurrent > PHOTOTHERAPY_CONSUMPTION_MAX) {
    HW_error += PHOTOTHERAPY_CONSUMPTION_MAX_ERROR;
    logln("[HW] -> Fail -> PHOTOTHERAPY current consumption is too high");
  }
  vTaskDelay(CURRENT_STABILIZE_TIME);
  if (HUMIDIFIER_MODE == HUMIDIFIER_PWM) {
    ledcWrite(HUMIDIFIER_PWM_CHANNEL, HUMIDIFIER_DUTY_CYCLE);
  }
  else if (HUMIDIFIER_MODE == HUMIDIFIER_BINARY) {
    GPIOWrite(HUMIDIFIER, HIGH);
  }
  vTaskDelay(CURRENT_STABILIZE_TIME);
  testCurrent = measureMeanConsumption(HUMIDIFIER_SHUNT); // <- UPDATE THIS CODE
  logln("[HW] -> Humidifier current consumption: " + String (testCurrent) + " Amps");
  GPRSSetPostVariables(NO_COMMENT, ",Hum:" + String (testCurrent));
  if (testCurrent < HUMIDIFIER_CONSUMPTION_MIN) {
    HW_error += HUMIDIFIER_CONSUMPTION_MIN_ERROR;
    logln("[HW] -> Fail -> HUMIDIFIER current consumption is too low");
  }
  if (testCurrent > HUMIDIFIER_CONSUMPTION_MAX) {
    HW_error += HUMIDIFIER_CONSUMPTION_MAX_ERROR;
    logln("[HW] -> Fail -> HUMIDIFIER current consumption is too high");
  }
  if (HUMIDIFIER_MODE == HUMIDIFIER_PWM) {
    ledcWrite(HUMIDIFIER_PWM_CHANNEL, false);
  }
  else if (HUMIDIFIER_MODE == HUMIDIFIER_BINARY) {
    GPIOWrite(HUMIDIFIER, LOW);
  }
  vTaskDelay(CURRENT_STABILIZE_TIME);
  offsetCurrent = measureMeanConsumption(FAN_SHUNT_CHANNEL);
  GPIOWrite(FAN, HIGH);
  vTaskDelay(CURRENT_STABILIZE_TIME);
  testCurrent = measureMeanConsumption(FAN_SHUNT_CHANNEL) - offsetCurrent;
  logln("[HW] -> FAN consumption: " + String (testCurrent) + " Amps");
  GPRSSetPostVariables(NO_COMMENT, ",Fan:" + String (testCurrent));
  if (testCurrent < FAN_CONSUMPTION_MIN) {
    HW_error += FAN_CONSUMPTION_MIN_ERROR;
    logln("[HW] -> Fail -> Fan current consumption is too low");
  }
  if (testCurrent > FAN_CONSUMPTION_MAX) {
    HW_error += FAN_CONSUMPTION_MAX_ERROR;
    logln("[HW] -> Fail -> Fan current consumption is too high");
  }
  GPIOWrite(FAN, LOW);
  vTaskDelay(CURRENT_STABILIZE_TIME);
  offsetCurrent = measureMeanConsumption(SYSTEM_SHUNT_CHANNEL);
  ledcWrite(HEATER_PWM_CHANNEL, PWM_MAX_VALUE);
  vTaskDelay(CURRENT_STABILIZE_TIME);
  testCurrent = measureMeanConsumption(SYSTEM_SHUNT_CHANNEL) - offsetCurrent;
  logln("[HW] -> Heater current consumption: " + String (testCurrent) + " Amps");
  GPRSSetPostVariables(NO_COMMENT, ",Hea:" + String (testCurrent));
  if (testCurrent < HEATER_CONSUMPTION_MIN) {
    HW_error += HEATER_CONSUMPTION_MIN_ERROR;
    logln("[HW] -> Fail -> Heater current consumption is too low");
  }
  if (testCurrent > HEATER_CONSUMPTION_MAX) {
    HW_error += HEATER_CONSUMPTION_MAX_ERROR;
    logln("[HW] -> Fail -> Heater current consumption is too high");
  }
  ledcWrite(HEATER_PWM_CHANNEL, 0);
  vTaskDelay(CURRENT_STABILIZE_TIME);
  if (error == HW_error) {
    logln("[HW] -> OK -> Actuators are working as expected");
  }
  else {
    logln("[HW] -> Fail -> Some actuators are not working as expected");
  }
}
