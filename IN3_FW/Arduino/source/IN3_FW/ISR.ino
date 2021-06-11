void peripheralsISR() {
  sensorsISR();
  buzzerISR();
  userInteraction();
}

void encSwitchISR() {
  encPulseDetected = 1;
  buzzerTone(buzzerStandbyToneTimes, buzzerSwitchDuration, buzzerRotaryEncoderTone);
  lastUserInteraction = millis();
}

void powerAlertISR() {
  if (millis() - lastPowerAlertNotification > powerAlertNotificationPeriod) {
    lastPowerAlertNotification = millis();
    powerAlert=1;
  }
}
