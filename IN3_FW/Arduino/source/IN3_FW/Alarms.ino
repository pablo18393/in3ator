void alarmTimerStart() {
  temperatureAlarmTime = millis();
  humidityAlarmTime = millis();
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
