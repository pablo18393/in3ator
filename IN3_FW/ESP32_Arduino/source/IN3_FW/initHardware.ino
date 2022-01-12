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
#define TFTCheckPeriod 10000
long HW_error = false;
long lastTFTCheck;

void initDebug() {
  Serial.begin(115200);
  delay(100);
  logln("in3ator debug uart, version " + String (FWversion) + ", SN: " + String (serialNumber));
}

void initHardware() {
  initDebug();
  logln("[HW] -> Initialiting hardware");
  initEEPROM();
  initI2CBus();
  initCurrentSensor();
  initExternalADC();
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
  watchdogInit();
}

void initInterrupts() {
  attachInterrupt(ENC_SWITCH, encSwitchISR, CHANGE);
  attachInterrupt(ENC_A, encoderISR, CHANGE);
  attachInterrupt(ENC_B, encoderISR, CHANGE);
}

void initI2CBus() {
  Wire.begin();
}

void initCurrentSensor() {
  digitalCurrentSensorPresent = false;
  Wire.beginTransmission(digitalCurrentSensor_i2c_address);
  if (!Wire.endTransmission()) {
    digitalCurrentSensorPresent = true;
    temperature_filter = digital_temperature_filter;
    PAC.begin();
    PAC.setSampleRate(1024);
    PAC.UpdateCurrent();
    logln("[HW] -> digital sensor detected, current consumption is " + String(float(PAC.Current / 1000), 2) + " Amperes");
  }
}

void initExternalADC() {
  externalADCpresent = false;
  long conversionTime;
  Wire.beginTransmission(digitalCurrentSensor_i2c_address);
  if (!Wire.endTransmission()) {
    logln("[HW] -> external ADC detected");
    externalADCpresent = true;
    mySensor.begin(externalADC_i2c_address);
    mySensor.setInputMultiplexer(ADS122C04_MUX_AIN0_AVSS); // Route AIN1 and AIN0 to AINP and AINN
    mySensor.setGain(ADS122C04_GAIN_1); // Set the gain to 1
    mySensor.enablePGA(ADS122C04_PGA_DISABLED); // Disable the Programmable Gain Amplifier
    mySensor.setDataRate(ADS122C04_DATA_RATE_1000SPS); // Set the data rate (samples per second) to 1000
    mySensor.setOperatingMode(ADS122C04_OP_MODE_NORMAL); // Disable turbo mode
    mySensor.setConversionMode(ADS122C04_CONVERSION_MODE_CONTINUOUS); // Use single shot mode
    mySensor.setVoltageReference(ADS122C04_VREF_INTERNAL); // Use the internal 2.048V reference
    mySensor.enableInternalTempSensor(ADS122C04_TEMP_SENSOR_OFF); // Disable the temperature sensor
    mySensor.setDataCounter(ADS122C04_DCNT_DISABLE); // Disable the data counter (Note: the library does not currently support the data count)
    mySensor.setDataIntegrityCheck(ADS122C04_CRC_DISABLED); // Disable CRC checking (Note: the library does not currently support data integrity checking)
    mySensor.setBurnOutCurrent(ADS122C04_BURN_OUT_CURRENT_OFF); // Disable the burn-out current
    mySensor.setIDACcurrent(ADS122C04_IDAC_CURRENT_OFF); // Disable the IDAC current
    mySensor.setIDAC1mux(ADS122C04_IDAC1_DISABLED); // Disable IDAC1
    mySensor.setIDAC2mux(ADS122C04_IDAC2_DISABLED); // Disable IDAC2
  }
  conversionTime = micros();
  mySensor.start(); // Start the conversion
  while (!mySensor.checkDataReady()) {
    delayMicroseconds(10);
  }
  logln("[HW] -> Baby NTC voltage is: " + String(float(mySensor.readADC()) / 4096000, 4) + ", conversion made in " + String(micros() - conversionTime) + " us");
  conversionTime = micros();
  mySensor.setInputMultiplexer(ADS122C04_MUX_AIN1_AVSS); // Route AIN1 and AIN0 to AINP and AINN
  mySensor.start(); // Start the conversion
  while (!mySensor.checkDataReady()) {
    delayMicroseconds(10);
  }
  logln("[HW] -> Air NTC voltage is: " + String(float(mySensor.readADC()) / 4096000, 4) + ", conversion made in " + String(micros() - conversionTime) + " us");
  conversionTime = micros();
  mySensor.setInputMultiplexer(ADS122C04_MUX_AIN2_AVSS); // Route AIN1 and AIN0 to AINP and AINN
  mySensor.start(); // Start the conversion
  while (!mySensor.checkDataReady()) {
    delayMicroseconds(10);
  }
  logln("[HW] -> Vin voltage is: " + String(float(mySensor.readADC()) / 4096000, 4) + ", conversion made in " + String(micros() - conversionTime) + " us");
  conversionTime = micros();
  mySensor.setInputMultiplexer(ADS122C04_MUX_AIN3_AVSS); // Route AIN1 and AIN0 to AINP and AINN
  mySensor.start(); // Start the conversion
  while (!mySensor.checkDataReady()) {
    delayMicroseconds(10);
  }
  logln("[HW] -> System current voltage is: " + String(float(mySensor.readADC()) / 4096000, 4) + ", conversion made in " + String(micros() - conversionTime) + " us");
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

void initializeTFT() {
  tft.begin(DISPLAY_CONTROLLER_IC);
  tft.setRotation(DISPLAY_DEFAULT_ROTATION);
}

void initTFT() {
  long error = HW_error;
  float testCurrent, offsetCurrent;
  int  timeOut = 4000;
  long processTime = millis();
  backlightPower = maxPWMvalue / screenBrightnessFactor;
  backlightPowerSafe = maxPWMvalue * backlightPowerSafePercentage;
  offsetCurrent = measureConsumptionForTime(testTime);
  initPin(TFT_CS, OUTPUT);
  GPIOWrite(TFT_CS, LOW);
  initializeTFT();
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
  initPin(TFT_CS, OUTPUT);
  GPIOWrite(TFT_CS, LOW);
  initializeTFT();
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

void buzzerHandler() {
  if (millis() - buzzerTime > buzzerToneTime && buzzerBeeps) {
    buzzerBeeps -= buzzerBuzzing;
    buzzerBuzzing = !buzzerBuzzing;
    ledcWrite(BUZZER_PWM_CHANNEL, buzzerMaxPWM / 2 * buzzerBuzzing);
    buzzerTime = millis();
  }
  if (page == actuatorsProgressPage) {
    buzzerStandbyTime = max(lastbacklightHandler, buzzerStandbyTime);
  }
}

void buzzerConstantTone (int freq) {
  ledcWrite(BUZZER_PWM_CHANNEL, buzzerMaxPWM / 2);
}

void shutBuzzer () {
  ledcWrite(BUZZER_PWM_CHANNEL, false);
}

void buzzerTone (int beepTimes, int timeDelay, int freq) {
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
