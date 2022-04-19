#define enableAlarms true
#define NOT_CRITICAL_ALARM 0
#define CRITICAL_ALARM 1

#define HUMIDITY_ALARM 0
#define TEMPERATURE_ALARM 1
#define AIR_THERMAL_CUTOUT_ALARM 2
#define SKIN_THERMAL_CUTOUT_ALARM 3
#define NUM_ALARMS 4

bool alarmOnGoing[NUM_ALARMS];

long lastAlarmTrigger[NUM_ALARMS];

float alarmSensedValue;

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
          disableAlarm(alarmID);
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
          disableAlarm(alarmID);
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

void resetAlarm(byte alarmID) {
  alarmOnGoing[alarmID] = false;
  lastAlarmTrigger[alarmID] = millis();
  drawAlarmMessage(ERASE, alarmID);
}

bool ongoingAlarms() {
  return (alarmOnGoing[TEMPERATURE_ALARM] || alarmOnGoing[HUMIDITY_ALARM] || alarmOnGoing[AIR_THERMAL_CUTOUT_ALARM] || alarmOnGoing[SKIN_THERMAL_CUTOUT_ALARM]);
}

bool ongoingThermalCutout() {
  return (!(alarmOnGoing[AIR_THERMAL_CUTOUT_ALARM] || alarmOnGoing[SKIN_THERMAL_CUTOUT_ALARM]));
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
    resetAlarm(TEMPERATURE_ALARM);
  }
  if (alarmOnGoing[HUMIDITY_ALARM]) {
    resetAlarm(HUMIDITY_ALARM);
  }
  if (alarmOnGoing[AIR_THERMAL_CUTOUT_ALARM]) {
    resetAlarm(AIR_THERMAL_CUTOUT_ALARM);
  }
  if (alarmOnGoing[SKIN_THERMAL_CUTOUT_ALARM]) {
    resetAlarm(SKIN_THERMAL_CUTOUT_ALARM);
  }
}
