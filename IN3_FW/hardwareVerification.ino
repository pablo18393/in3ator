byte hardwareVerification() {
  pinDirection();
  delayMicroseconds(mosfet_switch_time);
  bool testOK = 1;
  byte hardwareTests = 10;
  bool errorHardwareCode[hardwareTests];
  for (int i = 0; i < hardwareTests; i++) {
    errorHardwareCode[i] = 0;
  }
  if (!digitalRead(POWER_EN_FB) && HWPowerEn) {
    errorHardwareCode[0] = 1;
    criticalError = 1;
  }
  if (!digitalRead(HEATER_FB) && HWHeater) {
    errorHardwareCode[1] = 1;
    criticalError = 1;
  }
  if (!digitalRead(FAN1_FB) && HWFan1) {
    errorHardwareCode[2] = 1;
    criticalError = 1;
  }
  if (!digitalRead(FAN2_FB) && HWFan2) {
    errorHardwareCode[3] = 1;
    criticalError = 1;
  }
  if (!digitalRead(FAN3_FB) && HWFan3) {
    errorHardwareCode[4] = 1;
    criticalError = 1;
  }
  if (!digitalRead(ICT_FB) && HWICT) {
    errorHardwareCode[5] = 1;
    criticalError = 1;
  }
  if (!digitalRead(STERILIZE_FB) && HWSterilize) {
    errorHardwareCode[6] = 1;
    criticalError = 1;
  }
  if (!digitalRead(HUMIDIFIER_FB) && HWHumidifier) {
    errorHardwareCode[7] = 1;
    criticalError = 1;
  }
  //power verification
  if ((analogRead(THERMISTOR_HEATER) > 3200 || analogRead(THERMISTOR_HEATER) < 1200) && HWNTCHeater) {
    errorHardwareCode[8] = 1;
    criticalError = 1;
  }
  if ((analogRead(THERMISTOR_CORNER) > 3200 || analogRead(THERMISTOR_CORNER) < 1200) && HWNTCIn3) {
    errorHardwareCode[9] = 1;
    criticalError = 1;
  }
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
  for (int i = 0; i < hardwareTests; i++) {
    if (errorHardwareCode[i]) {
      testOK = 0;
      Serial.print("hardware error code: ");
      Serial.print(i);
      Serial.println(".");
    }
  }
  return (testOK);
}
