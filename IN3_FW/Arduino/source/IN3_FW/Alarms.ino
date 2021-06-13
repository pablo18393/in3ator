void alarmTimerStart() {
  temperatureAlarmTime = millis();
  humidityAlarmTime = millis();
}

void resetAlarms() {
  if (alarmOnGoing[temperatureAlarm]) {
    alarmOnGoing[temperatureAlarm] = 0;
    temperatureAlarmTime = millis();
  }
  if (alarmOnGoing[humidityAlarm]) {
    alarmOnGoing[humidityAlarm] = 0;
    humidityAlarmTime = millis();
  }
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
