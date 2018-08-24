byte hardwareVerification() {
  pinDirection();
  delay(mosfet_switch_time);
  byte errorHardwareCode = 0;
  if (!digitalRead(POWER_EN_FB)) {
    errorHardwareCode = 1;
  }
  if (!digitalRead(HEATER_FB)) {
    errorHardwareCode = 2;
  }
  if (!digitalRead(FAN1_FB)) {
    errorHardwareCode = 3;
  }
  if (!digitalRead(FAN2_FB)) {
    errorHardwareCode = 4;
  }
  if (!digitalRead(FAN3_FB)) {
    errorHardwareCode = 5;
  }
  if (!digitalRead(ICT_FB)) {
    errorHardwareCode = 6;
  }
  if (!digitalRead(STERILIZE_FB)) {
    errorHardwareCode = 7;
  }
  if (!digitalRead(HUMIDIFIER_FB)) {
    errorHardwareCode = 8;
  }
  //power verification
  if (analogRead(THERMISTOR_HEATER) > 800 || analogRead(THERMISTOR_HEATER) < 300) {
    errorHardwareCode = 9;
  }
  if (analogRead(THERMISTOR_CORNER) > 800 || analogRead(THERMISTOR_CORNER) < 300) {
    errorHardwareCode = 10;
  }
  digitalWrite(POWER_EN, HIGH);
  delay(mosfet_switch_time);
  if (!digitalRead(POWER_EN_FB)) {
    errorHardwareCode = 11;
  }
  digitalWrite(POWER_EN, LOW);
  digitalWrite(HEATER, HIGH);
  delay(mosfet_switch_time);
  if (!digitalRead(HEATER_FB)) {
    errorHardwareCode = 12;
  }
  digitalWrite(HEATER, LOW);
  digitalWrite(FAN1, HIGH);
  delay(mosfet_switch_time);
  if (!digitalRead(FAN1_FB)) {
    errorHardwareCode = 13;
  }
  digitalWrite(FAN1, LOW);
  digitalWrite(FAN2, HIGH);
  delay(mosfet_switch_time);
  if (!digitalRead(FAN2_FB)) {
    errorHardwareCode = 14;
  }
  digitalWrite(FAN2, LOW);
  digitalWrite(FAN3, HIGH);
  delay(mosfet_switch_time);
  if (!digitalRead(FAN3_FB)) {
    errorHardwareCode = 15;
  }
  digitalWrite(FAN3, LOW);
  digitalWrite(ICT, HIGH);
  delay(mosfet_switch_time);
  if (!digitalRead(ICT_FB)) {
    errorHardwareCode = 16;
  }
  digitalWrite(ICT, LOW);
  digitalWrite(STERILIZE, HIGH);
  delay(mosfet_switch_time);
  if (!digitalRead(STERILIZE_FB)) {
    errorHardwareCode = 17;
  }
  digitalWrite(STERILIZE, LOW);
  digitalWrite(HUMIDIFIER, HIGH);
  delay(mosfet_switch_time);
  if (!digitalRead(HUMIDIFIER_FB)) {
    errorHardwareCode = 18;
  }
  digitalWrite(HUMIDIFIER, LOW);
  if (errorHardwareCode) {

  }
  return (errorHardwareCode);
}
