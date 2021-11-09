void IRAM_ATTR peripheralsISR() {
  sensorsISR();
  buzzerISR();
  userInteraction();
}

void encSwitchISR() {
  if (millis() - lastEncPulse > encPulseDebounce) {
    statusEncSwitch = !statusEncSwitch;
    if (statusEncSwitch) {
      encPulseDetected = true;
      buzzerTone(buzzerStandbyToneTimes, buzzerSwitchDuration, buzzerRotaryEncoderTone);
      lastUserInteraction = millis();
      if (alarmOnGoing[temperatureAlarm] || alarmOnGoing[humidityAlarm]) {
        resetAlarms(); 
      }
    }
  }
  lastEncPulse = millis();
}

void encoderISR() {
  int newPos;
  encoder.tick(); // just call tick() to check the state.
  newPos = encoder.getPosition();
  lastUserInteraction = millis();
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
