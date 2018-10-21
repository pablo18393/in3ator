void hardwareVerification() {
  testOK = 1;
  testCritical = 0;
  shortcircuitTest();
  if (!testCritical) {
    sensorsTest();
    openCircuitTest();
  }
  for (int i = 0; i < hardwareComponents; i++) {
    if (errorHardwareCode[i]) {
      Serial.print("hardware error code: ");
      Serial.print(i);
      Serial.println(".");
    }
  }
}

void shortcircuitTest() {
  for (int i = 0; i < hardwareComponents - numNTC; i++) {
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
}

void openCircuitTest() {
  //power verification
  digitalWrite(POWER_EN, HIGH);
  delayMicroseconds(mosfet_switch_time);
  if (digitalRead(POWER_EN_FB) && HWPowerEn) {
    errorHardwareCode[0] = 1;
  }
  digitalWrite(POWER_EN, LOW);
  digitalWrite(HEATER, HIGH);
  delayMicroseconds(mosfet_switch_time);
  if (digitalRead(HEATER_FB) && HWHeater) {
    errorHardwareCode[1] = 1;
  }
  digitalWrite(HEATER, LOW);
  digitalWrite(FAN1, HIGH);
  delayMicroseconds(mosfet_switch_time);
  if (digitalRead(FAN1_FB) && HWFan1) {
    errorHardwareCode[2] = 1;
  }
  digitalWrite(FAN1, LOW);
  digitalWrite(FAN2, HIGH);
  delayMicroseconds(mosfet_switch_time);
  if (digitalRead(FAN2_FB) && HWFan2) {
    errorHardwareCode[3] = 1;
  }
  digitalWrite(FAN2, LOW);
  digitalWrite(FAN3, HIGH);
  delayMicroseconds(mosfet_switch_time);
  if (digitalRead(FAN3_FB) && HWFan3) {
    errorHardwareCode[4] = 1;
  }
  digitalWrite(FAN3, LOW);
  digitalWrite(ICT, HIGH);
  delayMicroseconds(mosfet_switch_time);
  if (digitalRead(ICT_FB) && HWICT) {
    errorHardwareCode[5] = 1;
  }
  digitalWrite(ICT, LOW);
  digitalWrite(STERILIZE, HIGH);
  delayMicroseconds(mosfet_switch_time);
  if (digitalRead(STERILIZE_FB) && HWSterilize) {
    errorHardwareCode[6] = 1;
  }
  digitalWrite(STERILIZE, LOW);
  digitalWrite(HUMIDIFIER, HIGH);
  delayMicroseconds(mosfet_switch_time);
  if (digitalRead(HUMIDIFIER_FB) && HWHumidifier) {
    errorHardwareCode[7] = 1;
  }
  digitalWrite(HUMIDIFIER, LOW);
}

