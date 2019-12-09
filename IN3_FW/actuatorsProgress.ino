
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
  checkTempSensorPin();
  if (swapTempSensors) {
    asignCorrectTempSensorsPin();
  }
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
      updateData();
      back_mode();
    }
    blinkGoBackMessage();
  }
}

void checkTempSensorPin() {
  //pending: program timeout too
  //logln("checking NTC pinout");
  float prevroomTemp = temperature[babyNTC];
  float prevHeaterTemp = temperature[heaterNTC];
  //logln(String(prevroomTemp));
  //logln(String(prevHeaterTemp));
  bool exitCheck = 0;
  long timeElapsedChecking = millis();
  turnFans(ON);
  heatUp();
  while (!exitCheck) {
    updateData();
    while (!digitalRead(ENC_SWITCH)) {
      updateData();
      back_mode();
    }
    if (temperature[babyNTC] > temperature[heaterNTC] + CheckSensorRaiseTemp) {
      swapTempSensors = 1;
      EEPROM.write(EEPROM_swapTempSensors, swapTempSensors);
      exitCheck = 1;
    }
    if (millis() - timeElapsedChecking > CheckTempSensorPinTimeout) {
      //error
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
    delay(500);
    printStatus();
  }
}


void asignCorrectTempSensorsPin() {
  int valueRetainer = THERMISTOR_HEATER;
  THERMISTOR_HEATER = THERMISTOR_ROOM;
  THERMISTOR_ROOM = valueRetainer;
  NTCpin[babyNTC] = {THERMISTOR_ROOM};
  NTCpin[heaterNTC] = {THERMISTOR_HEATER};
  //logln("NTC pins swapped");
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
    digitalWrite(HEATER, LOW);
  }
}

void basicHumidityControl() {
  if (humidity < desiredRoomHum) {
    digitalWrite(HUMIDIFIER, HIGH);
  }
  else {
    digitalWrite(HUMIDIFIER, LOW);
  }
}

void heatUp() {
  if (temperature[heaterNTC] < maxHeaterTemp) {
    heaterPower = HeatermaxPWM;
    digitalWrite(HEATER, HIGH);
  }
  else {
    digitalWrite(HEATER, LOW);
    heaterPower = 0;
  }
}

void turnActuators(bool mode) {
  digitalWrite(HEATER, mode);
  digitalWrite(HUMIDIFIER, mode);
}

void turnFans(bool mode) {
  digitalWrite(FAN_HP, mode);
  digitalWrite(FAN_LP, mode);
}

void turnSterilizeLED (bool mode) {
  digitalWrite(STERILIZE_CTL, mode);
}
