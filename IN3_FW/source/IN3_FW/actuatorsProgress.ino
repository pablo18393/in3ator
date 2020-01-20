void actuatorsProgress() {
  byte  numWords = 0;
  temperaturePercentage = 0;
  page = actuatorsProgressPage;
  tft.setTextSize(1);
  print_text = 0;
  rectangles = numWords;
  graphics();
  drawHeading();
  tft.setTextColor(COLOR_MENU_TEXT);
  setSensorsGraphicPosition();
  drawActuatorsSeparators();
  GPRSSetPostVariables(actuatorsModeON, "Actuators ON:" + String (desiredSkinTemp, 1) + "," + String (desiredRoomHum));
  setGPRSPostPeriod(actuatingGPRSPostPeriod);
  if (controlTemperature) {
    temperatureAtStart = temperature[babyNTC];
    printLoadingTemperatureBar();
    switch (language) {
      case spanish:
        textToWrite = "Temperatura";
        break;
      case english:
      case french:
        textToWrite = "Temperature";
        break;
    }
    tft.drawCentreString(textToWrite, tft.width() / 2, tempBarPosY - 4 * letter_height / 3, textFontSize);
  }
  if (controlHumidity) {
    humidityAtStart = humidity;
    printLoadingHumidityBar();
    switch (language) {
      case spanish:
        textToWrite = "Humedad";
        break;
      case english:
        textToWrite = "Humidity";
        break;
      case french:
        textToWrite = "Humidite";
        break;
    }
    tft.drawCentreString(textToWrite, tft.width() / 2, humBarPosY - 4 * letter_height / 3, textFontSize);
  }
  tft.setTextColor(COLOR_WARNING_TEXT);
  drawStop();
  state_blink = 1;
  while (!digitalRead(ENC_SWITCH));
  turnFans(ON);
  turnSterilizeLED (ON);
  if (temperatureAtStart > temperature[babyNTC]) {
    temperatureAtStart = temperature[babyNTC];
  }
  if (controlTemperature) {
    if (temperaturePIDcontrol) {
      startPID();
    }
  }
  while (1) {
    updateData();
    if (controlTemperature) {
      if (!temperaturePIDcontrol) {
        basictemperatureControl();
      }
    }
    if (controlHumidity) {
      basicHumidityControl();
    }
    while (!digitalRead(ENC_SWITCH)) {
      back_mode();
    }
    blinkGoBackMessage();
  }
}

void checkTempSensorPin() {
  float prevroomTemp = temperature[babyNTC];
  float prevHeaterTemp = temperature[heaterNTC];
  bool exitCheck = 0;
  long timeElapsedChecking = millis();
  turnFans(ON);
  heatUp();
  while (!exitCheck) {
    updateData();
    if (temperature[babyNTC] > temperature[heaterNTC] + CheckSensorRaiseTemp) {
      swapTempSensors = 1;
      EEPROM.write(EEPROM_swapTempSensors, swapTempSensors);
      exitCheck = 1;
    }
    if ((temperature[heaterNTC] - prevHeaterTemp) > CheckSensorRaiseTemp) {
      swapTempSensors = 0;
      exitCheck = 1;
    }
    if ((temperature[babyNTC] - prevroomTemp) > CheckSensorRaiseTemp) {
      swapTempSensors = 1;
      EEPROM.write(EEPROM_swapTempSensors, swapTempSensors);
      exitCheck = 1;
    }
    if (millis() - timeElapsedChecking > CheckTempSensorPinTimeout) {
      exitCheck = 1;
    }
    delay(500);
    printStatus();
    while (!digitalRead(ENC_SWITCH)) {
      back_mode();
    }
  }
}


void asignCorrectTempSensorsPin() {
  int valueRetainer = HEATER_NTC_PIN;
  HEATER_NTC_PIN = BABY_NTC_PIN;
  BABY_NTC_PIN = valueRetainer;
  logln("NTC pins swapped");
}

void blinkGoBackMessage() {
  if (millis() - blinking > 1000) {
    blinking = millis();
    state_blink = !state_blink;
    if (state_blink) {
      tft.setTextColor(ILI9341_ORANGE);
    }
    else {
      tft.setTextColor(COLOR_MENU);
      blinking += 400;
    }
    drawStop();
  }
}

void basictemperatureControl() {
  if (temperature[babyNTC] < desiredSkinTemp) {
    heatUp();
  }
  else {
    pwmWrite(HEATER, 0);
  }
}

void basicHumidityControl() {
  if (humidity < desiredRoomHum) {
    pwmWrite(HUMIDIFIER, humidifierMaxPWM / 2);
  }
  else {
    pwmWrite(HUMIDIFIER, 0);
  }
}

void heatUp() {
  if (temperature[heaterNTC] < maxHeaterTemp) {
    heaterPower = heaterMaxPWM;
  }
  else {
    heaterPower = 0;
  }
  pwmWrite(HEATER, heaterPower);
}

void turnActuators(bool mode) {
  pwmWrite(HEATER, mode * heaterMaxPWM);
  if (mode) {
    pwmWrite(HUMIDIFIER, humidifierMaxPWM / 2);
  }
  else {
    pwmWrite(HUMIDIFIER, 0);
  }
}

void turnFans(bool mode) {
  pwmWrite(FAN_HP, mode * fanHPMaxPWM);
  digitalWrite(FAN_LP, mode);
}

void turnSterilizeLED (bool mode) {
  digitalWrite(STERILIZE_CTL, mode);
}
