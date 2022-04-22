#define TEMPERATURE_ERROR  1 // 1 degrees difference to trigger alarm
#define HUMIDITY_ERROR  12 //12 %RH to trigger alarm

#define TEMPERATURE_ERROR_HYSTERESIS  0.05 // 0.05 degrees difference to disable alarm
#define HUMIDITY_ERROR_HYSTERESIS  5 //5 %RH to disable alarm

#define FAN_TEST_CURRENTDIF_MIN 0.4 //when the fan is spinning, heater cools down and consume less current
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

#define HUMIDITY_ALARM 0
#define TEMPERATURE_ALARM 1
#define AIR_THERMAL_CUTOUT_ALARM 2
#define SKIN_THERMAL_CUTOUT_ALARM 3
#define AIR_SENSOR_ISSUE_ALARM 4
#define SKIN_SENSOR_ISSUE_ALARM 5
#define FAN_ISSUE_ALARM 6
#define NUM_ALARMS 7

long lastSuccesfullSensorUpdate[numSensors];
#define MINIMUM_SUCCESSFULL_SENSOR_UPDATE 30000 //in millis

bool alarmOnGoing[NUM_ALARMS];
long lastAlarmTrigger[NUM_ALARMS];
float alarmSensedValue;

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
      evaluateAlarm(TEMPERATURE_ALARM, desiredControlTemp, alarmSensedValue, TEMPERATURE_ERROR, TEMPERATURE_ERROR_HYSTERESIS , lastAlarmTrigger[TEMPERATURE_ALARM]);
    }
    if (controlHumidity) {
      evaluateAlarm(HUMIDITY_ALARM, humidity, desiredRoomHum, HUMIDITY_ERROR, HUMIDITY_ERROR_HYSTERESIS, lastAlarmTrigger[HUMIDITY_ALARM]);
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
  if (millis() - alarmTime > ALARM_TIME_DELAY * 60 * 1000) { // min to millis
    if (errorMargin) {
      if ((abs(setPoint - measuredValue) + hysteresisValue * alarmOnGoing[alarmID]) > errorMargin) {
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
      if ((measuredValue + hysteresisValue * alarmOnGoing[alarmID]) > setPoint) {
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
}

bool ongoingAlarms() {
  return (alarmOnGoing[TEMPERATURE_ALARM] || alarmOnGoing[HUMIDITY_ALARM] || alarmOnGoing[AIR_THERMAL_CUTOUT_ALARM] || alarmOnGoing[SKIN_THERMAL_CUTOUT_ALARM] || alarmOnGoing[AIR_SENSOR_ISSUE_ALARM] || alarmOnGoing[SKIN_SENSOR_ISSUE_ALARM]);
}

bool ongoingCriticalAlarm() {
  return (!(alarmOnGoing[AIR_THERMAL_CUTOUT_ALARM] || alarmOnGoing[SKIN_THERMAL_CUTOUT_ALARM] || alarmOnGoing[AIR_SENSOR_ISSUE_ALARM] || alarmOnGoing[SKIN_SENSOR_ISSUE_ALARM]));
}

void setAlarm (byte alarmID) {
  alarmOnGoing[alarmID] = true;
  buzzerConstantTone(buzzerAlarmTone);
  drawAlarmMessage(DRAW, alarmIDtoString(alarmID));
}

void resetAlarm (byte alarmID) {
  alarmOnGoing[alarmID] = false;
  if (!ongoingAlarms()) {
    shutBuzzer();
    drawAlarmMessage(ERASE, alarmIDtoString(alarmID));
  }
}

void disableAllAlarms() {
  for (int i = 0; i < NUM_ALARMS; i++) {
    lastAlarmTrigger[i] = millis();
  }
  shutBuzzer();
}

char* alarmIDtoString (byte alarmID) {
  switch (alarmID) {
    case AIR_THERMAL_CUTOUT_ALARM:
    case SKIN_THERMAL_CUTOUT_ALARM:
      return ("THERMAL CUTOUT ALARM");
      break;
    case TEMPERATURE_ALARM:
      return ("TEMPERATURE ALARM");
      break;
    case HUMIDITY_ALARM:
      return ("HUMIDITY ALARM");
      break;
    case AIR_SENSOR_ISSUE_ALARM:
      return ("AIR SENSOR ALARM");
      break;
    case SKIN_SENSOR_ISSUE_ALARM:
      return ("SKIN SENSOR ALARM");
      break;
    case FAN_ISSUE_ALARM:
      return ("FAN ALARM");
      break;
    default:
      return ("ALARM");
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
    if (back_mode()) {
      return false;
    }
    updateData();
  }
  offsetCurrent = measureMeanConsumption(MAIN_SHUNT, defaultTestingSamples);
  ledcWrite(HEATER_PWM_CHANNEL, false);
  turnFans(ON);
  ongoingTest = millis();
  while (millis() - ongoingTest < FAN_TEST_PREHEAT_TIME) {
    if (back_mode()) {
      return false;
    }
    updateData();
  }
  ledcWrite(HEATER_PWM_CHANNEL, HEATER_HALF_PWR * ongoingCriticalAlarm());
  testCurrent = measureMeanConsumption(MAIN_SHUNT, defaultTestingSamples);
  Serial.println("FAN test: " + String(offsetCurrent - testCurrent));
  if (offsetCurrent - testCurrent < FAN_TEST_CURRENTDIF_MIN) {
    setAlarm(FAN_ISSUE_ALARM);
  }
  ledcWrite(HEATER_PWM_CHANNEL, false);
}
