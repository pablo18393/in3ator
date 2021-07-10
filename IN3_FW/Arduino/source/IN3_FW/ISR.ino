void peripheralsISR() {
  sensorsISR();
  buzzerISR();
  userInteraction();
}

void encSwitchISR() {
  encPulseDetected = 1;
  buzzerTone(buzzerStandbyToneTimes, buzzerSwitchDuration, buzzerRotaryEncoderTone);
  lastUserInteraction = millis();
  if (alarmOnGoing[temperatureAlarm] || alarmOnGoing[humidityAlarm]) {
    resetAlarms();
  }
}

void encoderISR() {
  int newPos;
  encoder.tick(); // just call tick() to check the state.
  newPos = encoder.getPosition();
  lastUserInteraction = millis();
  if (abs(newPos - last_encoder_move) > 1) {
    EncMove = EncMoveOrientation * int(encoder.getDirection());
    last_encoder_move = newPos;
  }
}

void powerAlertISR() {
  if (millis() - lastPowerAlertNotification > powerAlertNotificationPeriod) {
    lastPowerAlertNotification = millis();
    powerAlert = 1;
  }
}
