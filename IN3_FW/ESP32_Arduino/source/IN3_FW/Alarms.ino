#define enableAlarms true
#define NOT_CRITICAL_ALARM 0
#define CRITICAL_ALARM 1

#define HUMIDITY_ALARM 0
#define TEMPERATURE_ALARM 1
#define THERMAL_CUTOUT_ALARM 2

#define allAlarms 255
float alarmSensedValue;

void alarmTimerStart() {
  temperatureAlarmTime = millis();
  humidityAlarmTime = millis();
}

void resetTemperatureAlarm() {
  alarmOnGoing[TEMPERATURE_ALARM] = false;
  temperatureAlarmTime = millis();
  drawAlarmMessage(ERASE, TEMPERATURE_ALARM);
}

void resetHumidityAlarm() {
  alarmOnGoing[HUMIDITY_ALARM] = false;
  humidityAlarmTime = millis();
  drawAlarmMessage(ERASE, HUMIDITY_ALARM);
}

bool evaluateAlarm(byte alarmID, float setPoint, float measuredValue, int errorMargin, float hysteresisValue, long alarmTime, bool alarmPriority) {
  if (millis() - alarmTime > alarmTimeDelay * 60 * 1000 || alarmPriority == CRITICAL_ALARM) { // min to millis
    if (errorMargin) {
      if (abs(setPoint - measuredValue) > errorMargin) {
        if (!alarmOnGoing[alarmID]) {
          setAlarm(alarmID);
        }
        return true;
      }
      else {
        if (alarmOnGoing[alarmID]) {
          disableAlarm(alarmID);
        }
      }
    }
    else {
      if (measuredValue > setPoint) {
        if (!alarmOnGoing[alarmID]) {
          setAlarm(alarmID);
        }
        return true;
      }
      else {
        if (alarmOnGoing[alarmID]) {
          disableAlarm(alarmID);
        }
      }
    }
  }
  return false;
}

void setAlarm (byte alarmID) {
  alarmOnGoing[alarmID] = true;
  buzzerConstantTone(buzzerAlarmTone);
  drawAlarmMessage(DRAW, alarmID);
}


void disableAlarm (byte alarmID) {
  alarmOnGoing[alarmID] = false;
  shutBuzzer();
  drawAlarmMessage(ERASE, alarmID);
}

void resetAlarms() {
  if (alarmOnGoing[TEMPERATURE_ALARM]) {
    resetTemperatureAlarm();
  }
  if (alarmOnGoing[HUMIDITY_ALARM]) {
    resetHumidityAlarm();
  }
  resetHumidityAlarm();
}
