void initBoard() {
  watchdogInit();
  initDebug();
  initEEPROM();
  initHardware();
  //initSD();
  initTimers();
}

void initDebug() {
  Serial.begin(115200);
  delay(100);
  logln("in3ator debug uart, version " + String (FWversion) + ", SN: " + String (serialNumber));
}

void initInterrupts() {
  attachInterrupt(ENC_SWITCH, encSwitchISR, CHANGE);
  attachInterrupt(ENC_A, encoderISR, CHANGE);
  attachInterrupt(ENC_B, encoderISR, CHANGE);
}

void initTimers() {
  // Interval in microsecs
  /*
  if (ITimer0.attachInterruptInterval(peripheralsISRPeriod, peripheralsISR)) {
    Serial.print(F("Starting  ITimer0 OK, millis() = ")); Serial.println(millis());
  }
  else {
    Serial.println(F("Can't set ITimer0. Select another freq. or timer"));
  }
  */
}
