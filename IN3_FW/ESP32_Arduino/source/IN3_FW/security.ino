#define TEMPERATURE_ERROR  1 // 1 degrees difference to trigger alarm
#define HUMIDITY_ERROR  12 //12 %RH to trigger alarm

#define TEMPERATURE_ERROR_HYSTERESIS  0.05 // 0.05 degrees difference to disable alarm
#define HUMIDITY_ERROR_HYSTERESIS  5 //5 %RH to disable alarm



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
  if (millis() - lastSuccesfullSensorUpdate[sensor] > minimumSuccesfullSensorUpdate) {
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
  if (millis() - alarmTime > alarmTimeDelay * 60 * 1000) { // min to millis
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
  }
}
