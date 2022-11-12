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
extern SHTC3 mySHTC3;              // Declare an instance of the SHTC3 class
extern RotaryEncoder encoder;
extern Beastdevices_INA3221 digitalCurrentSensor;

extern int serialNumber;

extern bool WIFI_EN;
extern bool defaultWIFI_EN;
extern long lastDebugUpdate;
extern long loopCounts;
extern int page;
extern byte language;
extern int temperature_filter; //amount of temperature samples to filter
extern long lastNTCmeasurement, lastCurrentMeasurement, lastCurrentUpdate;

extern int NTC_PIN[numNTC];
extern double temperature[numSensors];
extern double errorTemperature[numSensors], temperatureCalibrationPoint;
extern double ReferenceTemperatureRange, ReferenceTemperatureLow;
extern double provisionalReferenceTemperatureLow;
extern double fineTuneSkinTemperature;
extern double RawTemperatureLow[numSensors], RawTemperatureRange[numSensors];
extern double provisionalRawTemperatureLow[numSensors];
extern double temperatureMaxReset;
extern double temperatureMinReset;
extern double temperatureMax[numSensors], temperatureMin[numSensors];
extern int temperatureArray [numNTC][analog_temperature_filter]; //variable to handle each NTC with the array of last samples (only for NTC)
extern int temperature_array_pos; //temperature sensor number turn to measure
extern float diffTemperature; //difference between measured temperature and user input real temperature
extern bool faultNTC[numNTC]; //variable to control a failure in NTC
extern double humidity; // room humidity variable
extern bool humidifierState, humidifierStateChange;
extern int previousHumidity; //previous sampled humidity
extern float diffHumidity; //difference between measured humidity and user input real humidity


extern byte autoCalibrationProcess;

//Sensor check rate (in ms). Both sensors are checked in same interrupt and they have different check rates
extern byte encoderRate;
extern byte encoderCount;
extern bool encPulseDetected;
extern volatile long lastEncPulse;
extern volatile bool statusEncSwitch;

//WIFI
extern bool WIFI_connection_status;

extern bool roomSensorPresent;
extern bool digitalCurrentSensorPresent;

extern float instantTemperature[secondOrder_filter];
extern float previousTemperature[secondOrder_filter];

//room variables
extern bool controlMode;
extern bool defaultcontrolMode;
extern bool controlAlgorithm;
extern bool defaultcontrolAlgorithm;
extern double desiredControlTemperature; //preset baby skin temperature
extern double desiredControlHumidity; //preset enviromental humidity
extern bool jaundiceEnable; //PWM that controls jaundice LED intensity
extern double desiredHeaterTemp; //desired temperature in heater

extern boolean A_set;
extern boolean B_set;
extern int encoderpinA; // pin  encoder A
extern int encoderpinB; // pin  encoder B
extern bool encPulsed, encPulsedBefore; //encoder switch status
extern bool updateUIData;
extern volatile int EncMove; //moved encoder
extern volatile int lastEncMove; //moved last encoder
extern volatile int EncMoveOrientation; //set to -1 to increase values clockwise
extern  int last_encoder_move; //moved encoder
extern long encoder_debounce_time; //in milliseconds, debounce time in encoder to filter signal bounces
extern long last_encPulsed; //last time encoder was pulsed

//Text Graphic position variables
extern int humidityX;
extern int humidityY;
extern int temperatureX;
extern int temperatureY;
extern int separatorTopYPos;
extern int separatorBotYPos;
extern bool controlTemperature;
extern bool controlHumidity;
extern int ypos;
extern bool print_text;
extern int initialSensorPosition;
extern bool pos_text[8];

extern bool enableSet;
extern float temperaturePercentage, temperatureAtStart;
extern float humidityPercentage, humidityAtStart;
extern int barWidth, barHeight, tempBarPosX, tempBarPosY, humBarPosX, humBarPosY;
extern int screenTextColor, screenTextBackgroundColor;

//User Interface display variables
extern bool autoLock; //setting that enables backlight switch OFF after a given time of no user actions
extern bool defaultAutoLock; //setting that enables backlight switch OFF after a given time of no user actions
extern long lastbacklightHandler; //last time there was a encoder movement or pulse
extern long sensorsUpdatePeriod;

extern bool selected;
extern char cstring[128];
extern char* textToWrite;
extern char* words[12];
extern char* helpMessage;
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


void sensorsHandler()
{
  measureNTCTemperature();
  // currentMonitor();
}

float measureMeanConsumption(int shunt)
{
  if (digitalCurrentSensorPresent)
  {
    return (digitalCurrentSensor.getCurrent(ina3221_ch_t(shunt))); // Amperes
  }
  return (false);
}

float adcToCelsius(float adcReading, int maxAdcReading)
{
  // Valores fijos del circuito
  float rAux = 10000.0;
  float vcc = 3.3;
  float beta = 3950.0;
  float temp0 = 298.0;
  float r0 = 10000.0;
  // Bloque de cálculo
  // Variables used in calculus
  float vm = 0.0;
  float rntc = 0.0;
  if (maxAdcReading && adcReading && (adcReading != maxAdcReading))
  {
    vm = (vcc) * (adcReading / maxAdcReading); // Calcular tensión en la entrada
    rntc = rAux / ((vcc / vm) - 1);            // Calcular la resistencia de la NTC
  }
  else
  {
    return false;
  }
  return (beta / (log(rntc / r0) + (beta / temp0)) - 273); // Calcular la temperatura en Celsius
}

bool measureNTCTemperature()
{
  float analogMeasurement;
  unsigned long start_time = millis(); // Record the start time so we can timeout
  int NTCmeasurement;
  if (micros() - lastNTCmeasurement > NTCMeasurementPeriod)
  {
    NTCmeasurement = analogRead(NTC_PIN[skinSensor]);
    if (NTCmeasurement > minimumAllowedNTCMeasurement && NTCmeasurement < maximumAllowedNTCMeasurement)
    {
      lastSuccesfullSensorUpdate[skinSensor] = millis();
      previousTemperature[0] = adcToCelsius(NTCmeasurement, maxADCvalue);
      temperature[skinSensor] = butterworth2(instantTemperature[1], instantTemperature[2], previousTemperature[0], previousTemperature[1], previousTemperature[2]);
      instantTemperature[0] = temperature[skinSensor];
      for (int i = 1; i >= 0; i--)
      {
        instantTemperature[i + 1] = instantTemperature[i];
        previousTemperature[i + 1] = previousTemperature[i];
      }
      errorTemperature[skinSensor] = temperature[skinSensor];
      if (RawTemperatureRange[skinSensor])
      {
        temperature[skinSensor] = (((temperature[skinSensor] - RawTemperatureLow[skinSensor]) * ReferenceTemperatureRange) / RawTemperatureRange[skinSensor]) + ReferenceTemperatureLow;
        temperature[skinSensor] += fineTuneSkinTemperature;
      }
      errorTemperature[skinSensor] -= temperature[skinSensor];
      if (temperature[skinSensor] < 0)
      {
        temperature[skinSensor] = 0;
      }
      if (temperature[skinSensor] > temperatureMax[skinSensor])
      {
        temperatureMax[skinSensor] = temperature[skinSensor];
      }
      if (temperature[skinSensor] < temperatureMin[skinSensor])
      {
        temperatureMin[skinSensor] = temperature[skinSensor];
      }
      lastNTCmeasurement = micros();
    }
    return true;
  }
  return false;
}

bool updateRoomSensor()
{
  if (roomSensorPresent)
  {
    noInterrupts();
    bool sensorState = mySHTC3.update();
    float sensedTemperature;
    logln("[SENSORS] -> Updating room humidity: state is " + String(sensorState));
    if (!sensorState)
    {
      sensedTemperature = mySHTC3.toDegC();
      if (sensedTemperature > minTempToDiscard && sensedTemperature < maxTempToDiscard)
      {
        lastSuccesfullSensorUpdate[airSensor] = millis();
        temperature[airSensor] = sensedTemperature; // Add here measurement to temp array
        humidity = mySHTC3.toPercent();
        if (temperature[airSensor] > temperatureMax[airSensor])
        {
          temperatureMax[airSensor] = temperature[airSensor];
        }
        if (temperature[airSensor] < temperatureMin[airSensor])
        {
          temperatureMin[airSensor] = temperature[airSensor];
        }
        return true;
      }
    }
    else
    {
      initRoomSensor();
    }
    interrupts();
  }
  else
  {
    initRoomSensor();
  }
  return false;
}
