void initBoard() {
  //watchdogInit();
  initDebug();
  //initEEPROM();
  initHardware();
  //initSD();
  initInterrupts();
  initTimers();
}

void initDebug() {
  Serial.begin(115200);
  delay(100);
  Serial.println("hello");
  logln("in3ator debug uart, version " + String (FWversion) + ", SN: " + String (serialNumber));
}

void initInterrupts() {
  //attachInterrupt(PWR_ALERT, powerAlertISR, RISING);
  attachInterrupt(ENC_SWITCH, encSwitchISR, FALLING);
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
