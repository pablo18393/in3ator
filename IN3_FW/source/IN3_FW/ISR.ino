void IRAM_ATTR encoderISR() {
  int newPos;
  encoder.tick(); // just call tick() to check the state.
  newPos = encoder.getPosition();
  lastbacklightHandler = millis();
  if (abs(lastEncMove - newPos) > ENCODER_TICKS_DIV) {
    EncMove = EncMoveOrientation * int(encoder.getDirection());
    lastEncMove = newPos;
  }
}

void IRAM_ATTR encSwitchHandler() {
  if (!digitalRead(ENC_SWITCH)) {
    if (millis() - lastEncPulse > encPulseDebounce) {
      if (!encPulseDetected) {
        buzzerTone(buzzerStandbyToneTimes, buzzerSwitchDuration, buzzerRotaryEncoderTone);
      }
      if (ongoingAlarms()) {
        disableAllAlarms();
      }
      encPulseDetected = true;
      //logln("[ENCODER] -> Pushed");
    }
    lastEncPulse = millis();
    lastbacklightHandler = millis();
  }
}
