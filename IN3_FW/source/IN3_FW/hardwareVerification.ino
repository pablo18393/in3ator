void hardwareVerification() {
  for (int i = 0; i < hardwareComponents; i++) {
    errorHardwareCode[i] = 0;
  }
  if (sensorsTest() && shortcircuitTest()) {
    if (openCircuitTest()) {
      logln("HARDWARE TEST OK");
    }
  }
  logln("HARDWARE TEST FAIL");
  drawHardwareErrorMessage();
  while (digitalRead(ENC_SWITCH));
}

void shortcircuitTest() {
  for (int i = 0; i < hardwareComponents - numSensors; i++) {
    if (i) {
      digitalWrite(POWER_EN, HIGH);
      delay(300); //wait for relay to switch
    }

  }
  digitalWrite(POWER_EN, LOW);
}

bool sensorsTest() {
  //sensors verification
  if (((HEATER_NTC_PIN) > 3200 || analogRead(HEATER_NTC_PIN) < 1200) && HWNTCHeater) {
    return (false);
  }
  if ((analogRead(BABY_NTC_PIN) > 3200 || analogRead(BABY_NTC_PIN) < 1200) && HWNTCRoom) {
    return (false);
  }
  //logln(String(updateHumidity()));
  if (HWHUMSensor && !updateHumidity()) {
    return (false);
  }
  return (true);
}

void openCircuitTest() {
  //power verification
  for (int i = 0; i < hardwareComponents - numSensors; i++) {
    digitalWrite(hardwareVerificationPin[i], HIGH);
    delayMicroseconds(mosfet_switch_time);
    if (digitalRead(hardwareVerificationPin_FB[i]) && hardwareMounted[i]) {
      errorHardwareCode[i] += opencircuit;
    }
    digitalWrite(hardwareVerificationPin[i], LOW);
  }
}
