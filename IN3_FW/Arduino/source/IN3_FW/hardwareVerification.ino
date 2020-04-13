void hardwareVerification() {
  bool testOK = 1;
  if (sensorsTest() && shortcircuitTest()) {
    if (openCircuitTest()) {
      logln("HARDWARE TEST OK");
    }
    else {
      testOK = 0;
    }
  }
  else {
    testOK = 0;
  }
  if (!testOK) {
    logln("HARDWARE TEST FAIL");
    drawHardwareErrorMessage();
    while (digitalRead(ENC_SWITCH));
  }
}

bool shortcircuitTest() {
  return true;
}

bool sensorsTest() {
  //sensors verification
  if ((HEATER_NTC_PIN) > 3200 || analogRead(HEATER_NTC_PIN) < 1200) {
    return (false);
  }
  if (analogRead(BABY_NTC_PIN) > 3200 || analogRead(BABY_NTC_PIN) < 1200) {
    return (false);
  }
  //logln(String(updateHumidity()));
  if (!updateHumidity()) {
    return (false);
  }
  return (true);
}

bool openCircuitTest() {
  return true;
}
