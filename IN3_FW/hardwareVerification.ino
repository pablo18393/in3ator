void hardwareVerification() {
  numSensors = numNTC + dhtSensor;
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
      Serial.print("hardware error code: ");
      Serial.print(errorComponent[i]);
      if (hardwareCritical[i] && errorHardwareCode[i] == shortcircuit) {
        Serial.print(", critical");
      }
      Serial.println();
    }
  }
  if (testOK) {
    Serial.println("HARDWARE TEST OK");
  }
  else {
    for (int i = 0; i < hardwareComponents; i++) {
      Serial.print(errorHardwareCode[i]);
    }
    Serial.println();
    Serial.println("HARDWARE TEST FAIL");
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

  if ((analogRead(THERMISTOR_HEATER) > 3200 || analogRead(THERMISTOR_HEATER) < 1200) && HWNTCHeater) {
    errorHardwareCode[HW_NUM_NTCHeater] = opencircuit;
  }
  if ((analogRead(THERMISTOR_ROOM) > 3200 || analogRead(THERMISTOR_ROOM) < 1200) && HWNTCRoom) {
    errorHardwareCode[HW_NUM_NTCRoom] = opencircuit;
  }
  Serial.println(readHumSensor());
  if (HWHUMSensor && !readHumSensor()) {
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

