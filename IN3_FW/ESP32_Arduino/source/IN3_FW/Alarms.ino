#define enableAlarms false

float alarmSensedValue;

void alarmTimerStart() {
  temperatureAlarmTime = millis();
  humidityAlarmTime = millis();
}

void resetAlarms() {
  if (alarmOnGoing[temperatureAlarm]) {
    resetTemperatureAlarm();
  }
  if (alarmOnGoing[humidityAlarm]) {
    resetHumidityAlarm();
  }
  resetHumidityAlarm();
}

void resetTemperatureAlarm() {
  alarmOnGoing[temperatureAlarm] = false;
  temperatureAlarmTime = millis();
  drawAlarmMessage(ERASE, temperatureAlarm);
}

void resetHumidityAlarm() {
  alarmOnGoing[humidityAlarm] = false;
  humidityAlarmTime = millis();
  drawAlarmMessage(ERASE, humidityAlarm);
}

bool checkAlarms(float setPoint, float measuredValue, int errorMargin, long alarmTime) {
  if (alarmsEnable) {
    if (millis() - alarmTime > alarmTimeDelay * 60 * 1000) { // min to millis
      if (abs(setPoint - measuredValue) > errorMargin) {
        if (enableAlarms) {
          return true;
        }
        else {
          return false;
        }
      }
    }
  }
  return false;
}
