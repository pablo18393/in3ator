
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
  EncMove = EncMoveOrientation * int(encoder.getDirection());
  last_encoder_move = newPos;
}

void powerAlertISR() {
  /*
    if (millis() - lastPowerAlertNotification > powerAlertNotificationPeriod) {
    lastPowerAlertNotification = millis();
    powerAlert = true;
    }
  */
}
