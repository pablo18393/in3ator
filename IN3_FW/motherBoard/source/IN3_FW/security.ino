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

#define TEMPERATURE_ERROR  1 // 1 degrees difference to trigger alarm
#define HUMIDITY_ERROR  12 //12 %RH to trigger alarm

#define TEMPERATURE_ERROR_HYSTERESIS  0.05 // 0.05 degrees difference to disable alarm
#define HUMIDITY_ERROR_HYSTERESIS  5 //5 %RH to disable alarm

#define FAN_TEST_CURRENTDIF_MIN 0.2 //when the fan is spinning, heater cools down and consume less current
#define FAN_TEST_PREHEAT_TIME 30000 //when the fan is spinning, heater cools down and consume less current


//Alarm variables
#define powerAlertNotificationPeriod 1000

long lastPowerAlertNotification;
bool powerAlert;

#define ALARM_TIME_DELAY 30 //in mins, time to check alarm
//security config
#define AIR_THERMAL_CUTOUT 38
#define SKIN_THERMAL_CUTOUT 40
#define AIR_THERMAL_CUTOUT_HYSTERESIS 1
#define SKIN_THERMAL_CUTOUT_HYSTERESIS 1
#define enableAlarms true



long lastSuccesfullSensorUpdate[numSensors];
#define MINIMUM_SUCCESSFULL_SENSOR_UPDATE 30000 //in millis

bool alarmOnGoing[NUM_ALARMS];
long lastAlarmTrigger[NUM_ALARMS];
float alarmSensedValue;

void initAlarms() {
  lastAlarmTrigger[AIR_THERMAL_CUTOUT_ALARM] = -1 * minsToMillis(ALARM_TIME_DELAY);
  lastAlarmTrigger[SKIN_THERMAL_CUTOUT_ALARM] = -1 * minsToMillis(ALARM_TIME_DELAY);
}

void securityCheck() {
  checkThermalCutOuts();
  checkAlarms();
  checkSensors();
}

void checkThermalCutOuts() {
  evaluateAlarm(AIR_THERMAL_CUTOUT_ALARM, AIR_THERMAL_CUTOUT, temperature[airSensor], false, AIR_THERMAL_CUTOUT_HYSTERESIS, lastAlarmTrigger[AIR_THERMAL_CUTOUT_ALARM]);
  evaluateAlarm(SKIN_THERMAL_CUTOUT_ALARM, SKIN_THERMAL_CUTOUT, temperature[skinSensor], false, SKIN_THERMAL_CUTOUT_HYSTERESIS, lastAlarmTrigger[SKIN_THERMAL_CUTOUT_ALARM]);
}

void checkAlarms() {
  if (page == actuatorsProgressPage) {
    if (controlTemperature) {
      if (controlMode) {
        alarmSensedValue = temperature[airSensor];
      }
      else {
        alarmSensedValue = temperature[skinSensor];
      }
      evaluateAlarm(TEMPERATURE_ALARM, desiredControlTemperature, alarmSensedValue, TEMPERATURE_ERROR, TEMPERATURE_ERROR_HYSTERESIS , lastAlarmTrigger[TEMPERATURE_ALARM]);
    }
    if (controlHumidity) {
      evaluateAlarm(HUMIDITY_ALARM, humidity, desiredControlHumidity, HUMIDITY_ERROR, HUMIDITY_ERROR_HYSTERESIS, lastAlarmTrigger[HUMIDITY_ALARM]);
    }
  }
}

void checkSensors() {
  checkStatusOfSensor(airSensor);
  checkStatusOfSensor(skinSensor);
}

void checkStatusOfSensor(byte sensor) {
  byte alarmID;
  switch (sensor) {
    case airSensor:
      alarmID = AIR_SENSOR_ISSUE_ALARM;
      break;
    case skinSensor:
      alarmID = SKIN_SENSOR_ISSUE_ALARM;
      break;
  }
  if (millis() - lastSuccesfullSensorUpdate[sensor] > MINIMUM_SUCCESSFULL_SENSOR_UPDATE) {
    if (!alarmOnGoing[alarmID]) {
      setAlarm(alarmID);
    }
  }
  else {
    if (alarmOnGoing[alarmID]) {
      resetAlarm(alarmID);
    }
  }
}

bool evaluateAlarm(byte alarmID, float setPoint, float measuredValue, float errorMargin, float hysteresisValue, long alarmTime) {
  if (millis() - alarmTime > minsToMillis(ALARM_TIME_DELAY) || alarmOnGoing[alarmID]) { // min to millis
    if (errorMargin) {
      if ((abs(setPoint - measuredValue) + hysteresisValue) > errorMargin) {
        if (!alarmOnGoing[alarmID]) {
          setAlarm(alarmID);
          return true;
        }
      }
      else {
        if (alarmOnGoing[alarmID]) {
          resetAlarm(alarmID);
        }
      }
    }
    else {
      if ((measuredValue + hysteresisValue) > setPoint) {
        if (!alarmOnGoing[alarmID]) {
          setAlarm(alarmID);
          return true;
        }
      }
      else {
        if (alarmOnGoing[alarmID]) {
          resetAlarm(alarmID);
        }
      }
    }
  }
  return false;
}

void alarmTimerStart() {
  for (int  i = 0; i < NUM_ALARMS; i++) {
    lastAlarmTrigger [i] = millis();
  }
  lastAlarmTrigger[AIR_THERMAL_CUTOUT_ALARM] = -1 * minsToMillis(ALARM_TIME_DELAY);
  lastAlarmTrigger[SKIN_THERMAL_CUTOUT_ALARM] = -1 * minsToMillis(ALARM_TIME_DELAY);
}

byte activeAlarm() {
  for (int i = 0; i < NUM_ALARMS; i++) {
    if (alarmOnGoing[i]) {
      return (i);
    }
  }
  return false;
}

bool ongoingAlarms() {
  return (alarmOnGoing[TEMPERATURE_ALARM] || alarmOnGoing[HUMIDITY_ALARM] || alarmOnGoing[AIR_THERMAL_CUTOUT_ALARM] || alarmOnGoing[SKIN_THERMAL_CUTOUT_ALARM] || alarmOnGoing[AIR_SENSOR_ISSUE_ALARM] || alarmOnGoing[SKIN_SENSOR_ISSUE_ALARM] || alarmOnGoing[HEATER_ISSUE_ALARM] || alarmOnGoing[FAN_ISSUE_ALARM]);
}

bool ongoingCriticalAlarm() {
  return (!(alarmOnGoing[AIR_THERMAL_CUTOUT_ALARM] || alarmOnGoing[SKIN_THERMAL_CUTOUT_ALARM] || alarmOnGoing[AIR_SENSOR_ISSUE_ALARM] || alarmOnGoing[SKIN_SENSOR_ISSUE_ALARM]|| alarmOnGoing[HEATER_ISSUE_ALARM]));
}

void setAlarm (byte alarmID) {
  logln("[ALARM] ->" + String (alarmIDtoString(alarmID)) + " has been triggered");
  alarmOnGoing[alarmID] = true;
  buzzerConstantTone(buzzerAlarmTone);
  drawAlarmMessage(alarmIDtoString(alarmID));
}

void resetAlarm (byte alarmID) {
  logln("[ALARM] ->" + String (alarmIDtoString(alarmID)) + " has been disable");
  alarmOnGoing[alarmID] = false;
  drawHeading(page, serialNumber, FWversion);
  if (!ongoingAlarms()) {
    shutBuzzer();
  }
  else {
    drawAlarmMessage(alarmIDtoString(activeAlarm()));
  }
}

void disableAllAlarms() {
  for (int i = 0; i < NUM_ALARMS; i++) {
    if (alarmOnGoing[i]) {
      if (millis() - lastAlarmTrigger[i] > minsToMillis(ALARM_TIME_DELAY)) {
        shutBuzzer();
      }
      lastAlarmTrigger[i] = millis();
    }
  }
}

char* alarmIDtoString (byte alarmID) {
  switch (alarmID) {
    case AIR_THERMAL_CUTOUT_ALARM:
    case SKIN_THERMAL_CUTOUT_ALARM:
      return convertStringToChar(cstring, "THERMAL CUTOUT ALARM");
      break;
    case TEMPERATURE_ALARM:
      return convertStringToChar(cstring, "TEMPERATURE ALARM");
      break;
    case HUMIDITY_ALARM:
      return convertStringToChar(cstring, "HUMIDITY ALARM");
      break;
    case AIR_SENSOR_ISSUE_ALARM:
      return convertStringToChar(cstring, "AIR SENSOR ALARM");
      break;
    case SKIN_SENSOR_ISSUE_ALARM:
      return convertStringToChar(cstring, "SKIN SENSOR ALARM");
      break;
    case FAN_ISSUE_ALARM:
      return convertStringToChar(cstring, "FAN ALARM");
      break;
    case HEATER_ISSUE_ALARM:
      return convertStringToChar(cstring, "HEATER ALARM");
      break;
    default:
      return convertStringToChar(cstring, "ALARM");
      break;
  }
}

bool checkFan() {

  float offsetCurrent;
  float testCurrent;
  long ongoingTest = millis();
  bool exitTest;
  ledcWrite(HEATER_PWM_CHANNEL, HEATER_HALF_PWR * ongoingCriticalAlarm());
  while (millis() - ongoingTest < FAN_TEST_PREHEAT_TIME) {
    if (encoderContinuousPress(page)) {
      return false;
    }
    updateData();
  }
  offsetCurrent = measureMeanConsumption(SYSTEM_SHUNT_CHANNEL);
  turnFans(ON);
  ongoingTest = millis();
  while (millis() - ongoingTest < FAN_TEST_PREHEAT_TIME) {
    if (encoderContinuousPress(page)) {
      return false;
    }
    updateData();
  }
  ledcWrite(HEATER_PWM_CHANNEL, HEATER_HALF_PWR * ongoingCriticalAlarm());
  testCurrent = measureMeanConsumption(SYSTEM_SHUNT_CHANNEL);
  ledcWrite(HEATER_PWM_CHANNEL, false);
  if (offsetCurrent - testCurrent < FAN_TEST_CURRENTDIF_MIN) {
    setAlarm(FAN_ISSUE_ALARM);
    return false;
  }
  return true;
}
