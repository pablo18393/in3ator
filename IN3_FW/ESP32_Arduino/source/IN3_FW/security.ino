#define TEMPERATURE_ERROR  1 // 1 degrees difference to trigger alarm
#define HUMIDITY_ERROR  12 //12 %RH to trigger alarm

#define TEMPERATURE_ERROR_HYSTERESIS  0.05 // 1 degrees difference to trigger alarm
#define HUMIDITY_ERROR_HYSTERESIS  5 //12 %RH to trigger alarm

#define AIR_THERMAL_CUTOUT 38
#define SKIN_THERMAL_CUTOUT 40

#define AIR_THERMAL_CUTOUT_HYSTERESIS 1
#define SKIN_THERMAL_CUTOUT_HYSTERESIS 1

void securityCheck() {
  checkThermalCutOuts();
  checkAlarms();
}

void checkThermalCutOuts() {
  if (controlMode) {
    evaluateAlarm(THERMAL_CUTOUT_ALARM, AIR_THERMAL_CUTOUT, temperature[airSensor], false, AIR_THERMAL_CUTOUT_HYSTERESIS, temperatureAlarmTime, CRITICAL_ALARM);
  }
  else {
    evaluateAlarm(THERMAL_CUTOUT_ALARM, SKIN_THERMAL_CUTOUT, temperature[babySensor], false, SKIN_THERMAL_CUTOUT_HYSTERESIS, temperatureAlarmTime, CRITICAL_ALARM);
  }
}

void checkAlarms() {
  if (page == actuatorsProgressPage) {
    if (controlTemperature) {
      if (controlMode) {
        alarmSensedValue = temperature[airSensor];
      }
      else {
        alarmSensedValue = temperature[babySensor];
      }
      evaluateAlarm(TEMPERATURE_ALARM, desiredControlTemp, alarmSensedValue, TEMPERATURE_ERROR, TEMPERATURE_ERROR_HYSTERESIS , temperatureAlarmTime, NOT_CRITICAL_ALARM);
    }
    if (controlHumidity) {
      evaluateAlarm(HUMIDITY_ALARM, humidity, desiredRoomHum, HUMIDITY_ERROR, HUMIDITY_ERROR_HYSTERESIS, humidityAlarmTime, NOT_CRITICAL_ALARM);
    }
  }
}
