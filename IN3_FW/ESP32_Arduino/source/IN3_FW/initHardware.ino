#define testMode false
#define operativeMode true

#define testTime 50

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
  vTaskDelay(100);
  logln("in3ator debug uart, version " + String (FWversion) + ", SN: " + String (serialNumber));
}

void initHardware (bool printOutputTest) {
  initGPIO();
  initDebug();
  logln("[HW] -> Initialiting hardware");
  initEEPROM();
  initI2CBus();
  //initCurrentSensor();
  //initExternalADC();
  initSensors();
  initSenseCircuit();
  initTFT();
  initPowerAlarm();
  initBuzzer();
  initInterrupts();
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
    if (printOutputTest) {
      drawHardwareErrorMessage(HW_error);
      while (GPIORead(ENC_SWITCH)) {
        updateData();
      }
    }
  }
  buzzerTone(2, buzzerStandbyToneDuration, buzzerStandbyTone);
  watchdogInit();
  initAlarms();
}

void initGPIO() {
  initPin(PHOTOTHERAPY, OUTPUT);
  initPin(GPRS_PWRKEY, OUTPUT);
  initPin(SYS_SHUNT, INPUT);
  initPin(encoderpinA, INPUT_PULLUP);
  initPin(encoderpinB, INPUT_PULLUP);
  initPin(ENC_SWITCH, INPUT_PULLUP);
  initPin(TFT_CS, OUTPUT);
  initPin(PHOTOTHERAPY, OUTPUT);
  initPin(FAN, OUTPUT);
  initPin(HUMIDIFIER, OUTPUT);
  initPin(ADC_READY, INPUT_PULLUP);
  initPin(HEATER, OUTPUT);
  initPin(BUZZER, OUTPUT);
  initPin(SCREENBACKLIGHT, OUTPUT);
  initPWMGPIO();
}

void initPWMGPIO() {
  ledcSetup(SCREENBACKLIGHT_PWM_CHANNEL, DEFAULT_PWM_FREQUENCY, DEFAULT_PWM_RESOLUTION);
  ledcSetup(HEATER_PWM_CHANNEL, HEATER_PWM_FREQUENCY, DEFAULT_PWM_RESOLUTION);
  ledcSetup(BUZZER_PWM_CHANNEL, DEFAULT_PWM_FREQUENCY, DEFAULT_PWM_RESOLUTION);
  if (HUMIDIFIER_MODE == HUMIDIFIER_PWM) {
    ledcSetup(HUMIDIFIER_PWM_CHANNEL, HUMIDIFIER_PWM_FREQUENCY, DEFAULT_PWM_RESOLUTION);
  }
  ledcAttachPin(SCREENBACKLIGHT, SCREENBACKLIGHT_PWM_CHANNEL);
  ledcAttachPin(HEATER, HEATER_PWM_CHANNEL);
  ledcAttachPin(BUZZER, BUZZER_PWM_CHANNEL);
  if (HUMIDIFIER_MODE == HUMIDIFIER_PWM) {
    ledcAttachPin(HUMIDIFIER, HUMIDIFIER_PWM_CHANNEL);
    ledcWrite(HUMIDIFIER_PWM_CHANNEL, false);
  }
  ledcWrite(SCREENBACKLIGHT_PWM_CHANNEL, PWM_MAX_VALUE);
  ledcWrite(HEATER_PWM_CHANNEL, false);
  ledcWrite(BUZZER_PWM_CHANNEL, false);
}

void initInterrupts() {
  //attachInterrupt(ENC_SWITCH, encSwitchHandler, CHANGE);
  attachInterrupt(ENC_A, encoderISR, CHANGE);
  attachInterrupt(ENC_B, encoderISR, CHANGE);
}

float readExternalADC() {
  return (mySensor.readADC());
}

bool ADCConversionReady () {
  return (!digitalRead(ADC_READY));
}

void initI2CBus() {
  Wire.begin();
  wire = &Wire;
  wire->setClock(I2C_CLK_SPEED * 1000UL);
}

void initCurrentSensor() {
  wire->beginTransmission(digitalCurrentSensor_i2c_address);
  digitalCurrentSensorPresent = !(wire->endTransmission());
  if (digitalCurrentSensorPresent) {
    PAC.begin();
    PAC.setSampleRate(1024);
    PAC.UpdateCurrent();
    logln("[HW] -> digital sensor detected, current consumption is " + String(float(PAC.Current / 1000), 2) + " Amperes");
  }
}

void initExternalADC() {
  long conversionTime;
  wire->beginTransmission(digitalCurrentSensor_i2c_address);
  externalADCpresent = !(wire->endTransmission());
  if (externalADCpresent) {
    logln("[HW] -> external ADC detected");
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
    conversionTime = micros();
    mySensor.start(); // Start the conversion
    while (!ADCConversionReady());
    logln("[HW] -> Baby NTC voltage is: " + String(float(readExternalADC()) / 4096000, 4) + ", conversion made in " + String(micros() - conversionTime) + " us");
    conversionTime = micros();
    mySensor.setInputMultiplexer(ADS122C04_MUX_AIN1_AVSS); // Route AIN1 and AIN0 to AINP and AINN
    mySensor.start(); // Start the conversion
    while (!ADCConversionReady());
    logln("[HW] -> Air NTC voltage is: " + String(float(readExternalADC()) / 4096000, 4) + ", conversion made in " + String(micros() - conversionTime) + " us");
    conversionTime = micros();
    mySensor.setInputMultiplexer(ADS122C04_MUX_AIN2_AVSS); // Route AIN1 and AIN0 to AINP and AINN
    mySensor.start(); // Start the conversion
    while (!ADCConversionReady());
    logln("[HW] -> Vin voltage is: " + String(float(readExternalADC()) / 4096000, 4) + ", conversion made in " + String(micros() - conversionTime) + " us");
    conversionTime = micros();
    mySensor.setInputMultiplexer(ADS122C04_MUX_AIN3_AVSS); // Route AIN1 and AIN0 to AINP and AINN
    mySensor.start(); // Start the conversion
    while (!ADCConversionReady());
    logln("[HW] -> System current voltage is: " + String(float(readExternalADC()) / 4096000, 4) + ", conversion made in " + String(micros() - conversionTime) + " us");
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
  measureOffsetConsumption();
}

void initSensors() {
  long error = HW_error;
  logln("[HW] -> Initialiting sensors");
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

void initRoomSensor() {
  logln("[HW] -> Detecting room sensor...");
  wire->beginTransmission(roomSensorAddress);
  roomSensorPresent = !(wire->endTransmission());
  if (roomSensorPresent == true) {
    logln("[HW] -> Room sensor succesfully found, initializing...");
    mySHTC3.begin(Wire);
  }
}

void standByCurrentTest() {
  long error = HW_error;
  float testCurrent;
  logln("[HW] -> Measuring standby current...");

  testCurrent = measureMeanConsumption(MAIN_SHUNT, defaultTestingSamples);
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
  long processTime;
  if (screenBrightnessFactor) {
    backlightPower = PWM_MAX_VALUE / screenBrightnessFactor;
  }
  backlightPowerSafe = PWM_MAX_VALUE * backlightPowerSafePercentage;
  offsetCurrent = measureMeanConsumption(MAIN_SHUNT, defaultTestingSamples);
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
  testCurrent = measureMeanConsumption(MAIN_SHUNT, defaultTestingSamples) - offsetCurrent;
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

  offsetCurrent = measureMeanConsumption(MAIN_SHUNT, defaultTestingSamples);
  ledcWrite(BUZZER_PWM_CHANNEL, BUZZER_MAX_PWM / 2);

  testCurrent = measureMeanConsumption(MAIN_SHUNT, defaultTestingSamples) - offsetCurrent;
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

void actuatorsTest() {
  long error = HW_error;
  float testCurrent, offsetCurrent;

  offsetCurrent = measureMeanConsumption(MAIN_SHUNT, defaultTestingSamples);
  logln("[HW] -> Checking actuators...");

  GPIOWrite(PHOTOTHERAPY, HIGH);
  testCurrent = measureMeanConsumption(MAIN_SHUNT, defaultTestingSamples) - offsetCurrent;
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
  if (HUMIDIFIER_MODE == HUMIDIFIER_PWM) {
    ledcWrite(HUMIDIFIER_PWM_CHANNEL, HUMIDIFIER_DUTY_CYCLE);
  }
  else {
    GPIOWrite(HUMIDIFIER, HIGH);
  }
  testCurrent = measureMeanConsumption(HUMIDIFIER_SHUNT, defaultTestingSamples);
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
  if (HUMIDIFIER_MODE == HUMIDIFIER_PWM) {
    ledcWrite(HUMIDIFIER_PWM_CHANNEL, false);
  }
  else {
    GPIOWrite(HUMIDIFIER, LOW);
  }
  GPIOWrite(FAN, HIGH);
  testCurrent = measureMeanConsumption(MAIN_SHUNT, defaultTestingSamples) - offsetCurrent;
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

  ledcWrite(HEATER_PWM_CHANNEL, PWM_MAX_VALUE);
  testCurrent = measureMeanConsumption(MAIN_SHUNT, defaultTestingSamples) - offsetCurrent;
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
