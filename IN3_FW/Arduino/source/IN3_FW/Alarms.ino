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
  alarmOnGoing[temperatureAlarm] = 0;
  temperatureAlarmTime = millis();
  drawAlarmMessage(ERASE, temperatureAlarm);
}

void resetHumidityAlarm() {
  alarmOnGoing[humidityAlarm] = 0;
  humidityAlarmTime = millis();
  drawAlarmMessage(ERASE, humidityAlarm);
}

bool checkAlarms(float setValue, float measuredValue, int errorMargin, long alarmTime) {
  if (alarmsEnable) {
    if (millis() - alarmTime > alarmTimeDelay * 60 * 1000) { // min to millis
      if (abs(setValue - measuredValue) > errorMargin) {
        return true;
      }
    }
  }
  return false;
}
