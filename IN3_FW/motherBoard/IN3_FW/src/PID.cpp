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
extern long lastDebugUpdate;
extern long loopCounts;
extern int page;
extern int temperature_filter; // amount of temperature samples to filter
extern long lastNTCmeasurement[numNTC], lastCurrentMeasurement, lastCurrentUpdate;

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

// room variables;
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

extern in3ator_parameters in3;

void PIDInit()
{
  airControlPID.SetMode(MANUAL);
  skinControlPID.SetMode(MANUAL);
  humidityControlPID.SetMode(MANUAL);
}

void PIDHandler()
{
  if (airControlPID.GetMode() == AUTOMATIC)
  {
    airControlPIDInput = in3.temperature[airSensor];
    airControlPID.Compute();
    ledcWrite(HEATER_PWM_CHANNEL, HeaterPIDOutput * ongoingCriticalAlarm());
  }
  if (skinControlPID.GetMode() == AUTOMATIC)
  {
    skinControlPIDInput = in3.temperature[skinSensor];
    skinControlPID.Compute();
    ledcWrite(HEATER_PWM_CHANNEL, HeaterPIDOutput * ongoingCriticalAlarm());
  }
  if (humidityControlPID.GetMode() == AUTOMATIC)
  {
    humidityControlPID.Compute();
    if (millis() - windowStartTime > humidifierTimeCycle)
    { // time to shift the Relay Window
      windowStartTime += humidifierTimeCycle;
    }
    if (humidityControlPIDOutput < millis() - windowStartTime)
    {
      if (humidifierState || humidifierStateChange)
      {
        in3_hum.turn(OFF);
        humidifierStateChange = false;
      }
      humidifierState = false;
    }
    else
    {
      if (!humidifierState || humidifierStateChange)
      {
        in3_hum.turn(ON);
        humidifierStateChange = false;
      }
      humidifierState = true;
    }
  }
}

void startPID(byte var)
{
  switch (var)
  {
  case airPID:
    airControlPID.SetOutputLimits(false, BUZZER_MAX_PWR);
    airControlPID.SetTunings(Kp[airPID], Ki[airPID], Kd[airPID]);
    airControlPID.SetControllerDirection(DIRECT);
    airControlPID.SetMode(AUTOMATIC);
    break;
  case skinPID:
    skinControlPID.SetOutputLimits(false, BUZZER_MAX_PWR);
    skinControlPID.SetTunings(Kp[skinPID], Ki[skinPID], Kd[skinPID]);
    skinControlPID.SetControllerDirection(DIRECT);
    skinControlPID.SetMode(AUTOMATIC);
    break;
  case humidityPID:
    humidifierStateChange = true;
    windowStartTime = millis();
    humidityControlPID.SetOutputLimits(humidifierTimeCycle * humidifierDutyCycleMin / 100, humidifierTimeCycle * humidifierDutyCycleMax / 100);
    humidityControlPID.SetMode(AUTOMATIC);
    break;
  }
}

void stopPID(byte var)
{
  switch (var)
  {
  case airPID:
    airControlPID.SetMode(MANUAL);
    break;
  case skinPID:
    skinControlPID.SetMode(MANUAL);
    break;
  case humidityPID:
    humidityControlPID.SetMode(MANUAL);
  }
}
