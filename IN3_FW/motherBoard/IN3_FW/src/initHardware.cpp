/*
  MIT License

  Copyright (c) 2022 Medical Open World, Pablo Sánchez Bergasa

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

*/
#include <Arduino.h>
#include "main.h"

extern TwoWire *wire;
extern MAM_in3ator_Humidifier in3_hum;
extern Adafruit_ILI9341 tft;
extern SHTC3 mySHTC3; // Declare an instance of the SHTC3 class
extern RotaryEncoder encoder;
extern Beastdevices_INA3221 digitalCurrentSensor;

extern bool WIFI_EN;
extern bool defaultWIFI_EN;
extern long lastDebugUpdate;
extern long loopCounts;
extern int page;
extern int temperature_filter; // amount of temperature samples to filter
extern long lastNTCmeasurement, lastCurrentMeasurement, lastCurrentUpdate;

extern int NTC_PIN[numNTC];
extern double errorTemperature[numSensors], temperatureCalibrationPoint;
extern double ReferenceTemperatureRange, ReferenceTemperatureLow;
extern double provisionalReferenceTemperatureLow;
extern double fineTuneSkinTemperature;
extern double RawTemperatureLow[numSensors], RawTemperatureRange[numSensors];
extern double provisionalRawTemperatureLow[numSensors];
extern double temperatureMax[numSensors], temperatureMin[numSensors];
extern int temperatureArray[numNTC][analog_temperature_filter]; // variable to handle each NTC with the array of last samples (only for NTC)
extern int temperature_array_pos;                               // temperature sensor number turn to measure
extern float diffTemperature;                                   // difference between measured temperature and user input real temperature
extern bool humidifierState, humidifierStateChange;
extern int previousHumidity; // previous sampled humidity
extern float diffHumidity;   // difference between measured humidity and user input real humidity

extern byte autoCalibrationProcess;

// Sensor check rate (in ms). Both sensors are checked in same interrupt and they have different check rates
extern byte encoderRate;
extern byte encoderCount;
extern bool encPulseDetected;
extern volatile long lastEncPulse;
extern volatile bool statusEncSwitch;

// WIFI
extern bool WIFI_connection_status;

extern bool roomSensorPresent;
extern bool digitalCurrentSensorPresent;

extern float instantTemperature[secondOrder_filter];
extern float previousTemperature[secondOrder_filter];

// room variables
extern bool controlAlgorithm;
extern bool defaultcontrolAlgorithm;

extern boolean A_set;
extern boolean B_set;
extern int encoderpinA;                 // pin  encoder A
extern int encoderpinB;                 // pin  encoder B
extern bool encPulsed, encPulsedBefore; // encoder switch status
extern bool updateUIData;
extern volatile int EncMove;            // moved encoder
extern volatile int lastEncMove;        // moved last encoder
extern volatile int EncMoveOrientation; // set to -1 to increase values clockwise
extern int last_encoder_move;           // moved encoder
extern long encoder_debounce_time;      // in milliseconds, debounce time in encoder to filter signal bounces
extern long last_encPulsed;             // last time encoder was pulsed

// Text Graphic position variables
extern int humidityX;
extern int humidityY;
extern int temperatureX;
extern int temperatureY;
extern int separatorTopYPos;
extern int separatorBotYPos;
extern int ypos;
extern bool print_text;
extern int initialSensorPosition;
extern bool pos_text[8];

extern bool enableSet;
extern float temperaturePercentage, temperatureAtStart;
extern float humidityPercentage, humidityAtStart;
extern int barWidth, barHeight, tempBarPosX, tempBarPosY, humBarPosX, humBarPosY;
extern int screenTextColor, screenTextBackgroundColor;

// User Interface display variables
extern bool autoLock;             // setting that enables backlight switch OFF after a given time of no user actions
extern bool defaultAutoLock;      // setting that enables backlight switch OFF after a given time of no user actions
extern long lastbacklightHandler; // last time there was a encoder movement or pulse
extern long sensorsUpdatePeriod;

extern bool selected;
extern char cstring[128];
extern char *textToWrite;
extern char *words[12];
extern char *helpMessage;
extern byte bar_pos;
extern byte menu_rows;
extern byte length;
extern long lastGraphicSensorsUpdate;
extern long lastSensorsUpdate;
extern bool enableSetProcess;
extern long blinking;
extern bool state_blink;
extern bool blinkSetMessageState;
extern long lastBlinkSetMessage;

extern bool powerAlert;
extern long lastSuccesfullSensorUpdate[numSensors];

extern double HeaterPIDOutput;
extern double skinControlPIDInput;
extern double airControlPIDInput;
extern double humidityControlPIDOutput;
extern int humidifierTimeCycle;
extern unsigned long windowStartTime;

extern double Kp[numPID], Ki[numPID], Kd[numPID];
extern PID airControlPID;
extern PID skinControlPID;
extern PID humidityControlPID;

extern int ScreenBacklightMode;

#define testMode false
#define operativeMode true

#define CURRENT_STABILIZE_TIME_DEFAULT 300
#define CURRENT_STABILIZE_TIME_HEATER 800

#define NTC_BABY_MIN 1
#define NTC_BABY_MAX 60
#define DIG_TEMP_ROOM_MIN 1
#define DIG_TEMP_ROOM_MAX 60
#define DIG_HUM_ROOM_MIN 1
#define DIG_HUM_ROOM_MAX 100

#define HEATER_CONSUMPTION_MIN 2.5
#define FAN_CONSUMPTION_MIN 0.03
#define PHOTOTHERAPY_CONSUMPTION_MIN 0.3
#define HUMIDIFIER_CONSUMPTION_MIN 0.07

#define HEATER_CONSUMPTION_MAX 8
#define FAN_CONSUMPTION_MAX 1
#define PHOTOTHERAPY_CONSUMPTION_MAX 3
#define HUMIDIFIER_CONSUMPTION_MAX 0.8

#define STANDBY_CONSUMPTION_MIN 0
#define STANDBY_CONSUMPTION_MAX 1

#define SCREEN_CONSUMPTION_MIN 0.005
#define SCREEN_CONSUMPTION_MAX 1

#define BUZZER_CONSUMPTION_MIN 0

#define NTC_BABY_MIN_ERROR 1
#define NTC_BABY_MAX_ERROR 1 << 1
#define DIG_TEMP_ROOM_MIN_ERROR 1 << 2
#define DIG_TEMP_ROOM_MAX_ERROR 1 << 3
#define DIG_HUM_ROOM_MIN_ERROR 1 << 4
#define DIG_HUM_ROOM_MAX_ERROR 1 << 5
#define DIGITAL_SENSOR_NOTFOUND 1 << 6

#define HEATER_CONSUMPTION_MIN_ERROR 1 << 7
#define FAN_CONSUMPTION_MIN_ERROR 1 << 8
#define PHOTOTHERAPY_CONSUMPTION_MIN_ERROR 1 << 9
#define HUMIDIFIER_CONSUMPTION_MIN_ERROR 1 << 10

#define HEATER_CONSUMPTION_MAX_ERROR 1 << 11
#define FAN_CONSUMPTION_MAX_ERROR 1 << 12
#define PHOTOTHERAPY_CONSUMPTION_MAX_ERROR 1 << 13
#define HUMIDIFIER_CONSUMPTION_MAX_ERROR 1 << 14

#define STANDBY_CONSUMPTION_MAX_ERROR 1 << 15

#define DEFECTIVE_SCREEN 1 << 17
#define DEFECTIVE_BUZZER 1 << 18
#define DEFECTIVE_CURRENT_SENSOR 1 << 19

#define I2C_CLK_SPEED 50 // 50k

#define TFTCheckPeriod 10000
long HW_error = false;
long lastTFTCheck;

extern in3ator_parameters in3;

void initDebug()
{
  Serial.begin(115200);
  vTaskDelay(CURRENT_STABILIZE_TIME_DEFAULT);
  logln("in3ator debug uart, version v" + String(FWversion) + "/" + String(HWversion) + ", SN: " + String(in3.serialNumber));
}

void initPWMGPIO()
{
  ledcSetup(SCREENBACKLIGHT_PWM_CHANNEL, DEFAULT_PWM_FREQUENCY, DEFAULT_PWM_RESOLUTION);
  ledcSetup(HEATER_PWM_CHANNEL, HEATER_PWM_FREQUENCY, DEFAULT_PWM_RESOLUTION);
  ledcSetup(BUZZER_PWM_CHANNEL, DEFAULT_PWM_FREQUENCY, DEFAULT_PWM_RESOLUTION);
  ledcAttachPin(SCREENBACKLIGHT, SCREENBACKLIGHT_PWM_CHANNEL);
  ledcAttachPin(HEATER, HEATER_PWM_CHANNEL);
  ledcAttachPin(BUZZER, BUZZER_PWM_CHANNEL);
  ledcWrite(SCREENBACKLIGHT_PWM_CHANNEL, false);
  ledcWrite(HEATER_PWM_CHANNEL, false);
  ledcWrite(BUZZER_PWM_CHANNEL, false);
}

void initGPIO()
{
  pinMode(PHOTOTHERAPY, OUTPUT);
  pinMode(GPRS_PWRKEY, OUTPUT);
  pinMode(encoderpinA, INPUT_PULLUP);
  pinMode(encoderpinB, INPUT_PULLUP);
  pinMode(ENC_SWITCH, INPUT_PULLUP);
  pinMode(TFT_CS, OUTPUT);
  pinMode(PHOTOTHERAPY, OUTPUT);
  pinMode(FAN, OUTPUT);
  pinMode(HEATER, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(SCREENBACKLIGHT, OUTPUT);
  pinMode(ACTUATORS_EN, INPUT_PULLUP);
  pinMode(ON_OFF_SWITCH, INPUT);
  initPWMGPIO();
}

void initInterrupts()
{
  attachInterrupt(ENC_SWITCH, encSwitchHandler, CHANGE);
  attachInterrupt(ENC_A, encoderISR, CHANGE);
  attachInterrupt(ENC_B, encoderISR, CHANGE);
}

void initRoomSensor()
{
  roomSensorPresent = false;
  wire->beginTransmission(roomSensorAddress);
  roomSensorPresent = !(wire->endTransmission());
  if (roomSensorPresent == true)
  {
    logln("[HW] -> Room sensor succesfully found, initializing...");
    mySHTC3.begin(Wire);
  }
}

void initCurrentSensor()
{
  wire->beginTransmission(digitalCurrentSensor_i2c_address);
  digitalCurrentSensorPresent = !(wire->endTransmission());
  if (digitalCurrentSensorPresent)
  {
    digitalCurrentSensor.begin();
    digitalCurrentSensor.reset();
    // Set shunt resistors to 10 mOhm for all channels
    digitalCurrentSensor.setShuntRes(SYSTEM_SHUNT, PHOTOTHERAPY_SHUNT, FAN_SHUNT);
    digitalCurrentSensor.setShuntConversionTime(INA3221_REG_CONF_CT_140US);
    digitalCurrentSensor.setAveragingMode(INA3221_REG_CONF_AVG_128);
    logln("[HW] -> digital sensor detected, current consumption is " + String(digitalCurrentSensor.getCurrent(INA3221_CH1), 2) + " Amperes");
  }
  else
  {
    logln("[HW] -> no digital sensor detected");
  }
}

void initPowerAlarm()
{
}

void initSensors()
{
  long error = HW_error;
  logln("[HW] -> Initialiting sensors");
  Wire.begin(I2C_SDA, I2C_SCL);
  wire = &Wire;
  initCurrentSensor();
  initRoomSensor();
  // sensors verification
  while (!measureNTCTemperature())
    ;
  if (in3.temperature[skinSensor] < NTC_BABY_MIN)
  {
    logln("[HW] -> Fail -> NTC temperature is lower than expected");
    HW_error += NTC_BABY_MIN_ERROR;
  }
  if (in3.temperature[skinSensor] > NTC_BABY_MAX)
  {
    logln("[HW] -> Fail -> NTC temperature is higher than expected");
    HW_error += NTC_BABY_MAX_ERROR;
  }
  if (updateRoomSensor())
  {
    if (in3.temperature[airSensor] < DIG_TEMP_ROOM_MIN)
    {
      logln("[HW] -> Fail -> Room temperature is lower than expected");
      HW_error += DIG_TEMP_ROOM_MIN_ERROR;
    }
    if (in3.temperature[airSensor] > DIG_TEMP_ROOM_MAX)
    {
      logln("[HW] -> Fail -> Room temperature is higher than expected");
      HW_error += DIG_TEMP_ROOM_MAX_ERROR;
    }
    if (in3.humidity < DIG_HUM_ROOM_MIN)
    {
      logln("[HW] -> Fail -> Room humidity is lower than expected");
      HW_error += DIG_HUM_ROOM_MIN_ERROR;
    }
    if (in3.humidity > DIG_HUM_ROOM_MAX)
    {
      logln("[HW] -> Fail -> Room humidity is higher than expected");
      HW_error += DIG_HUM_ROOM_MAX_ERROR;
    }
  }
  else
  {
    HW_error += DIGITAL_SENSOR_NOTFOUND;
    logln("[HW] -> Fail -> No room sensor found");
  }
  if (error == HW_error)
  {
    logln("[HW] -> OK -> Sensors are working as expected");
  }
}

void standByCurrentTest()
{
  long error = HW_error;
  float testCurrent;
  logln("[HW] -> Measuring standby current...");

  testCurrent = measureMeanConsumption(SYSTEM_SHUNT_CHANNEL);
  if (testCurrent < STANDBY_CONSUMPTION_MIN)
  {
    HW_error += DEFECTIVE_CURRENT_SENSOR;
    logln("[HW] -> Fail -> Defective current sensor");
  }
  if (testCurrent > STANDBY_CONSUMPTION_MAX)
  {
    HW_error += STANDBY_CONSUMPTION_MAX_ERROR;
    logln("[HW] -> Fail -> Maximum stanby current exceeded");
  }
  if (error == HW_error)
  {
    logln("[HW] -> OK -> Current sensor is working as expected: " + String(testCurrent) + " Amps");
  }
  else
  {
    logln("[HW] -> Fail -> test current is " + String(testCurrent) + " Amps");
  }
  in3.system_current_standby_test = testCurrent;
}

void initSenseCircuit()
{
  standByCurrentTest();
}

void initializeTFT()
{
  tft.setController(ST7789V_CONTROLLER);
  tft.begin(DISPLAY_SPI_CLK);
  tft.setRotation(DISPLAY_DEFAULT_ROTATION);
}

void initTFT()
{
  long error = HW_error;
  float testCurrent, offsetCurrent;
  long processTime;
  offsetCurrent = measureMeanConsumption(SYSTEM_SHUNT_CHANNEL);
  digitalWrite(TFT_CS, LOW);
  initializeTFT();
  loadlogo();
  processTime = millis();
  for (int i = 0; i < BACKLIGHT_POWER_DEFAULT; i++)
  {
    ledcWrite(SCREENBACKLIGHT_PWM_CHANNEL, i);
    if (brightenRate)
    {
      while (millis() - processTime < i / brightenRate)
      {
        sensorsHandler();
      }
    }
    processTime = millis();
  }
  testCurrent = measureMeanConsumption(SYSTEM_SHUNT_CHANNEL) - offsetCurrent;
  if (testCurrent < SCREEN_CONSUMPTION_MIN)
  {
    // HW_error += DEFECTIVE_SCREEN;
    logln("[HW] -> WARNING -> Screen current is not high enough");
  }
  if (testCurrent > SCREEN_CONSUMPTION_MAX)
  {
    // HW_error += DEFECTIVE_SCREEN;
    logln("[HW] -> WARNING -> Screen current exceeded");
  }
  if (error == HW_error)
  {
    logln("[HW] -> OK -> Screen is working as expected: " + String(testCurrent) + " Amps");
  }
  else
  {
    logln("[HW] -> Fail -> test current is " + String(testCurrent) + " Amps");
  }
  in3.display_current_test = testCurrent;
}

void TFTRestore()
{
  logln("[HW] -> restoring TFT previous status");
  digitalWrite(TFT_CS, LOW);
  initializeTFT();
  switch (page)
  {
  case actuatorsProgressPage:
    UI_actuatorsProgress();
    break;
  case mainMenuPage:
  default:
    UI_mainMenu();
    break;
  }
}

void initBuzzer()
{
  long error = HW_error;
  float testCurrent, offsetCurrent;

  offsetCurrent = measureMeanConsumption(SYSTEM_SHUNT_CHANNEL);
  ledcWrite(BUZZER_PWM_CHANNEL, BUZZER_MAX_PWM / 2);
  vTaskDelay(CURRENT_STABILIZE_TIME_DEFAULT);
  testCurrent = measureMeanConsumption(SYSTEM_SHUNT_CHANNEL) - offsetCurrent;
  ledcWrite(BUZZER_PWM_CHANNEL, false);
  vTaskDelay(CURRENT_STABILIZE_TIME_DEFAULT);
  if (testCurrent < BUZZER_CONSUMPTION_MIN)
  {
    HW_error += DEFECTIVE_BUZZER;
    logln("[HW] -> Fail -> Buzzer current is not high enough");
  }
  if (error == HW_error)
  {
    logln("[HW] -> OK -> Buzzer is working as expected: " + String(testCurrent) + " Amps");
  }
  else
  {
    logln("[HW] -> Fail -> test current is " + String(testCurrent) + " Amps");
  }
  in3.buzzer_current_test = testCurrent;
}

bool actuatorsTest()
{
  logln("[HW] -> Checking actuators...");
  long error = HW_error;
  float testCurrent, offsetCurrent;
  offsetCurrent = measureMeanConsumption(SYSTEM_SHUNT_CHANNEL);
  ledcWrite(HEATER_PWM_CHANNEL, PWM_MAX_VALUE);
  vTaskDelay(CURRENT_STABILIZE_TIME_HEATER);
  testCurrent = measureMeanConsumption(SYSTEM_SHUNT_CHANNEL) - offsetCurrent;
  logln("[HW] -> Heater current consumption: " + String(testCurrent) + " Amps");
  in3.heater_current_test = testCurrent;
  ledcWrite(HEATER_PWM_CHANNEL, 0);
  if (testCurrent < HEATER_CONSUMPTION_MIN)
  {
    HW_error += HEATER_CONSUMPTION_MIN_ERROR;
    logln("[HW] -> Fail -> Heater current consumption is too low");
    setAlarm(HEATER_ISSUE_ALARM);
    return (true);
  }
  if (testCurrent > HEATER_CONSUMPTION_MAX)
  {
    HW_error += HEATER_CONSUMPTION_MAX_ERROR;
    logln("[HW] -> Fail -> Heater current consumption is too high");
    setAlarm(HEATER_ISSUE_ALARM);
    return (true);
  }
  vTaskDelay(CURRENT_STABILIZE_TIME_DEFAULT);
  offsetCurrent = measureMeanConsumption(PHOTOTHERAPY_SHUNT_CHANNEL);
  digitalWrite(PHOTOTHERAPY, HIGH);
  vTaskDelay(CURRENT_STABILIZE_TIME_DEFAULT);
  testCurrent = measureMeanConsumption(PHOTOTHERAPY_SHUNT_CHANNEL) - offsetCurrent;
  digitalWrite(PHOTOTHERAPY, LOW);
  logln("[HW] -> Phototherapy current consumption: " + String(testCurrent) + " Amps");
  in3.phototherapy_current_test = testCurrent;
  if (testCurrent < PHOTOTHERAPY_CONSUMPTION_MIN)
  {
    HW_error += PHOTOTHERAPY_CONSUMPTION_MIN_ERROR;
    logln("[HW] -> Fail -> PHOTOTHERAPY current consumption is too low");
  }
  if (testCurrent > PHOTOTHERAPY_CONSUMPTION_MAX)
  {
    HW_error += PHOTOTHERAPY_CONSUMPTION_MAX_ERROR;
    logln("[HW] -> Fail -> PHOTOTHERAPY current consumption is too high");
    return (true);
  }
  vTaskDelay(CURRENT_STABILIZE_TIME_DEFAULT);
  offsetCurrent = measureMeanConsumption(SYSTEM_SHUNT_CHANNEL); // <- UPDATE THIS CODE TO ASK I2C DATA
  in3_hum.turn(ON);
  vTaskDelay(CURRENT_STABILIZE_TIME_DEFAULT);
  testCurrent = measureMeanConsumption(SYSTEM_SHUNT_CHANNEL) - offsetCurrent; // <- UPDATE THIS CODE TO ASK I2C DATA
  logln("[HW] -> Humidifier current consumption: " + String(testCurrent) + " Amps");
  in3.humidifier_current_test = testCurrent;
  in3_hum.turn(OFF);
  if (testCurrent < HUMIDIFIER_CONSUMPTION_MIN)
  {
    HW_error += HUMIDIFIER_CONSUMPTION_MIN_ERROR;
    logln("[HW] -> Fail -> HUMIDIFIER current consumption is too low");
  }
  if (testCurrent > HUMIDIFIER_CONSUMPTION_MAX)
  {
    HW_error += HUMIDIFIER_CONSUMPTION_MAX_ERROR;
    logln("[HW] -> Fail -> HUMIDIFIER current consumption is too high");
    return (true);
  }
  vTaskDelay(CURRENT_STABILIZE_TIME_DEFAULT);
  offsetCurrent = measureMeanConsumption(FAN_SHUNT_CHANNEL);
  digitalWrite(FAN, HIGH);
  vTaskDelay(CURRENT_STABILIZE_TIME_DEFAULT);
  testCurrent = measureMeanConsumption(FAN_SHUNT_CHANNEL) - offsetCurrent;
  logln("[HW] -> FAN consumption: " + String(testCurrent) + " Amps");
  in3.fan_current_test = testCurrent;
  digitalWrite(FAN, LOW);
  if (testCurrent < FAN_CONSUMPTION_MIN)
  {
    HW_error += FAN_CONSUMPTION_MIN_ERROR;
    logln("[HW] -> Fail -> Fan current consumption is too low");
    return (true);
  }
  if (testCurrent > FAN_CONSUMPTION_MAX)
  {
    HW_error += FAN_CONSUMPTION_MAX_ERROR;
    logln("[HW] -> Fail -> Fan current consumption is too high");
    return (true);
  }
  if (error == HW_error)
  {
    logln("[HW] -> OK -> Actuators are working as expected");
  }
  else
  {
    logln("[HW] -> Fail -> Some actuators are not working as expected");
  }
  return (false);
}

bool initActuators()
{
  in3_hum.begin();
  return (actuatorsTest());
}

void checkTFTHealth()
{
  if (millis() - lastTFTCheck > TFTCheckPeriod)
  {
    uint8_t powerMode = tft.readcommand8(ILI9341_RDMODE);
    uint8_t MADCTL = tft.readcommand8(ILI9341_RDMADCTL);
    uint8_t pixelFormat = tft.readcommand8(ILI9341_RDPIXFMT);
    uint8_t imageFormat = tft.readcommand8(ILI9341_RDIMGFMT);
    uint8_t selfDiag = tft.readcommand8(ILI9341_RDSELFDIAG);
    logln("[HW] -> TFT display status is: " + String(powerMode, HEX) + String(MADCTL, HEX) + String(pixelFormat, HEX) + String(imageFormat, HEX) + String(selfDiag, HEX));
    if (!powerMode && !MADCTL && !pixelFormat && !imageFormat && !selfDiag)
    {
      TFTRestore();
    }
    lastTFTCheck = millis();
  }
}

void initHardware(bool printOutputTest)
{
  bool criticalError = false;
  initDebug();
  initEEPROM();
  initGPIO();
  logln("[HW] -> Initialiting hardware");
  initSensors();
  initSenseCircuit();
  initTFT();
  initPowerAlarm();
  initBuzzer();
  initInterrupts();
  PIDInit();
  criticalError = initActuators();
  criticalError = false;
  initGPRS();
  if (WIFI_EN)
  {
    wifiInit();
  }
  if (!HW_error)
  {
    logln("[HW] -> HARDWARE OK");
  }
  else
  {
    logln("[HW] -> HARDWARE TEST FAIL");
    logln("[HW] -> HARDWARE ERROR CODE:" + String(HW_error, HEX));
  }
  in3.HW_test_error_code = HW_error;
  if (printOutputTest || criticalError)
  {
    drawHardwareErrorMessage(HW_error, criticalError);
    while (digitalRead(ENC_SWITCH))
    {
      updateData();
    }
  }
  buzzerTone(2, buzzerStandbyToneDuration, buzzerStandbyTone);
  watchdogInit();
  initAlarms();
  // MQTT_init();
}