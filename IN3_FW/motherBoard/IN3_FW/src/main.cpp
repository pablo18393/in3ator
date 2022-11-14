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

// Firmware version and head title of UI screen

#include <Arduino.h>
#include "main.h"

TwoWire *wire;
MAM_in3ator_Humidifier in3_hum(DEFAULT_ADDRESS);
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
SHTC3 mySHTC3; // Declare an instance of the SHTC3 class
RotaryEncoder encoder(ENC_A, ENC_B, RotaryEncoder::LatchMode::TWO03);
Beastdevices_INA3221 digitalCurrentSensor(INA3221_ADDR41_VCC);

bool WIFI_EN = true;
bool defaultWIFI_EN = ON;
long lastDebugUpdate;
long loopCounts;
int page;
int temperature_filter = analog_temperature_filter; // amount of temperature samples to filter
long lastNTCmeasurement, lastCurrentMeasurement, lastCurrentUpdate;

int NTC_PIN[numNTC] = {BABY_NTC_PIN};
double errorTemperature[numSensors], temperatureCalibrationPoint;
double ReferenceTemperatureRange, ReferenceTemperatureLow;
double provisionalReferenceTemperatureLow;
double fineTuneSkinTemperature;
double RawTemperatureLow[numSensors], RawTemperatureRange[numSensors];
double provisionalRawTemperatureLow[numSensors];
double temperatureMax[numSensors], temperatureMin[numSensors];
int temperatureArray[numNTC][analog_temperature_filter]; // variable to handle each NTC with the array of last samples (only for NTC)
int temperature_array_pos;                               // temperature sensor number turn to measure
float diffTemperature;                                   // difference between measured temperature and user input real temperature
bool humidifierState, humidifierStateChange;
int previousHumidity; // previous sampled humidity
float diffHumidity;   // difference between measured humidity and user input real humidity

byte autoCalibrationProcess;

// Sensor check rate (in ms). Both sensors are checked in same interrupt and they have different check rates
byte encoderRate = true;
byte encoderCount = false;
bool encPulseDetected;
volatile long lastEncPulse;
volatile bool statusEncSwitch;

bool roomSensorPresent = false;
bool digitalCurrentSensorPresent = false;

float instantTemperature[secondOrder_filter];
float previousTemperature[secondOrder_filter];

// room variables
bool controlAlgorithm;
bool defaultcontrolAlgorithm = PID_CONTROL;
float minDesiredTemp[2] = {35, 30};   // minimum allowed temperature to be set
float maxDesiredTemp[2] = {37.5, 37}; // maximum allowed temperature to be set
int presetTemp[2] = {36, 32};         // preset baby skin temperature

boolean A_set;
boolean B_set;
int encoderpinA = ENC_A;         // pin  encoder A
int encoderpinB = ENC_B;         // pin  encoder B
bool encPulsed, encPulsedBefore; // encoder switch status
bool updateUIData;
volatile int EncMove;                 // moved encoder
volatile int lastEncMove;             // moved last encoder
volatile int EncMoveOrientation = -1; // set to -1 to increase values clockwise
volatile int last_encoder_move;       // moved encoder
long encoder_debounce_time = true;    // in milliseconds, debounce time in encoder to filter signal bounces
long last_encPulsed;                  // last time encoder was pulsed

// Text Graphic position variables
int humidityX;
int humidityY;
int temperatureX;
int temperatureY;
int separatorTopYPos;
int separatorBotYPos;
int ypos;
bool print_text;
int initialSensorPosition = separatorPosition - letter_width;
bool pos_text[8];

bool enableSet;
float temperaturePercentage, temperatureAtStart;
float humidityPercentage, humidityAtStart;
int barWidth, barHeight, tempBarPosX, tempBarPosY, humBarPosX, humBarPosY;
int screenTextColor, screenTextBackgroundColor;

// User Interface display variables
bool autoLock;             // setting that enables backlight switch OFF after a given time of no user actions
bool defaultAutoLock = ON; // setting that enables backlight switch OFF after a given time of no user actions
long lastbacklightHandler; // last time there was a encoder movement or pulse
long sensorsUpdatePeriod = 1000;

bool selected;
char cstring[128];
char *textToWrite;
char *words[12];
char *helpMessage;
byte bar_pos = true;
byte menu_rows;
byte length;
long lastGraphicSensorsUpdate;
long lastSensorsUpdate;
bool enableSetProcess;
long blinking;
bool state_blink;
bool blinkSetMessageState;
long lastBlinkSetMessage;

bool powerAlert;
long lastSuccesfullSensorUpdate[numSensors];

double Kp[numPID] = {200, 200, 200}, Ki[numPID] = {1, 1, 2}, Kd[numPID] = {500, 500, 20};
double HeaterPIDOutput;
double skinControlPIDInput;
double airControlPIDInput;
double humidityControlPIDOutput;
int humidifierTimeCycle = 5000;
unsigned long windowStartTime;

int ScreenBacklightMode;

in3ator_parameters in3;

PID airControlPID(&airControlPIDInput, &HeaterPIDOutput, &in3.desiredControlTemperature, Kp[airPID], Ki[airPID], Kd[airPID], P_ON_E, DIRECT);
PID skinControlPID(&skinControlPIDInput, &HeaterPIDOutput, &in3.desiredControlTemperature, Kp[skinPID], Ki[skinPID], Kd[skinPID], P_ON_E, DIRECT);
PID humidityControlPID(&in3.humidity, &humidityControlPIDOutput, &in3.desiredControlHumidity, Kp[humidityPID], Ki[humidityPID], Kd[humidityPID], P_ON_E, DIRECT);

void GPRS_Task(void *pvParameters)
{
  for (;;)
  {
    GPRS_Handler();
    vTaskDelay(GPRS_TASK_PERIOD / portTICK_PERIOD_MS);
  }
}

void setup()
{
  initHardware(false);
  UI_mainMenu();
  // Task generation
  /* Task n#1 - GPRS Handler */
  logln("Creating GPRS task ...\n");
  while (xTaskCreatePinnedToCore(GPRS_Task, (const char *)"GPRS", 4096, NULL, 1, NULL, 0) != pdPASS)
    ;
  ;
  logln("GPRS task successfully created!\n");
}

void loop()
{
  userInterfaceHandler(page);
  updateData();
}