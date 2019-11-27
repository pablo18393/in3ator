void hardwareVerification() {
  numSensors = numNTC + DHTSensor;
  testOK = 1;
  testCritical = 0;
  for (int i = 0; i < hardwareComponents; i++) {
    errorHardwareCode[i] = 0;
  }
  shortcircuitTest();
  sensorsTest();
  if (!testCritical) {
    openCircuitTest();
  }
  for (int i = 0; i < hardwareComponents; i++) {
    if (errorHardwareCode[i]) {
      testOK = 0;
      //log("hardware error code: ");
      //log(errorComponent[i]);
      if (hardwareCritical[i] && errorHardwareCode[i] == shortcircuit) {
        //log(", critical");
      }
      //logln("");
    }
  }
  if (testOK) {
    //logln("HARDWARE TEST OK");
  }
  else {
    for (int i = 0; i < hardwareComponents; i++) {
      //log(errorHardwareCode[i]);
    }
    //logln("");
    //logln("HARDWARE TEST FAIL");
    drawHardwareErrorMessage();
  }
}

void shortcircuitTest() {
  for (int i = 0; i < hardwareComponents - numSensors; i++) {
    if (i) {
      digitalWrite(POWER_EN, HIGH);
    }
    errorHardwareCode[i] = 0;
    if (!digitalRead(hardwareVerificationPin[i]) && hardwareMounted[i]) {
      errorHardwareCode[i] = shortcircuit;
      if (hardwareCritical[i]) {
        testCritical = 1;
      }
    }
  }
  digitalWrite(POWER_EN, LOW);
}

void sensorsTest() {
  //sensors verification

  if (((THERMISTOR_HEATER) > 3200 || analogRead(THERMISTOR_HEATER) < 1200) && HWNTCHeater) {
    errorHardwareCode[HW_NUM_NTCHeater] = opencircuit;
  }
  if ((analogRead(THERMISTOR_ROOM) > 3200 || analogRead(THERMISTOR_ROOM) < 1200) && HWNTCRoom) {
    errorHardwareCode[HW_NUM_NTCRoom] = opencircuit;
  }
  //logln(String(updateHumidity()));
  if (HWHUMSensor && !updateHumidity()) {
    errorHardwareCode[HW_NUM_HUMSensor] = opencircuit;
  }
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
