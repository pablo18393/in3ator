
void encSwitchISR() {
  if (!digitalRead(ENC_SWITCH)) {
    if (millis() - lastEncPulse > encPulseDebounce) {
      //statusEncSwitch = !statusEncSwitch;
      //if (statusEncSwitch) {
        encPulseDetected = true;
        buzzerTone(buzzerStandbyToneTimes, buzzerSwitchDuration, buzzerRotaryEncoderTone);
        lastbacklightHandler = millis();
        if (alarmOnGoing[temperatureAlarm] || alarmOnGoing[humidityAlarm]) {
          resetAlarms();
        }
      }
    //}
    lastEncPulse = millis();
  }
}

void encoderISR() {
  int newPos;
  encoder.tick(); // just call tick() to check the state.
  newPos = encoder.getPosition();
  lastbacklightHandler = millis();
  if (abs(lastEncMove - newPos) > ENCODER_TICKS_DIV) {
    EncMove = EncMoveOrientation * int(encoder.getDirection());
    lastEncMove = newPos;
  }
}

void powerAlertISR() {
  /*
    if (millis() - lastPowerAlertNotification > powerAlertNotificationPeriod) {
    lastPowerAlertNotification = millis();
    powerAlert = true;
    }
  */
}
