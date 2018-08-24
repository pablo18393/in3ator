void actuatorsProgress() {
  byte  numWords = 0;
  temperatureAtStart = temperature[cornerNTC];
  processPercentage = 0;
  page = actuatorsProgressPage;
  tft.setTextSize(1);
  print_text = 0;
  rectangles = numWords;
  drawGraphicInterface();
  drawHeading();
  tft.setTextColor(COLOR_MENU_TEXT);
  setSensorsGraphicPosition();
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
  tft.drawCentreString(textToWrite, tft.width() / 2, tft.height() / 5 - 12, textFontSize);
  tft.drawCentreString("%", tft.width() / 2 + 14, temperatureY , textFontSize);
  drawActuatorsSeparators();
  /*
    printLoadingHumidityBar();
    switch (language) {
      case spanish:
        textToWrite = "Humedad";
        break;
      case english:
        tft.textToWrite = "Humidity";
        break;
      case french:
        tft.textToWrite = "Humidite";
        break;
    }
    tft.drawCentreString(textToWrite, tft.width() / 2, tft.height() / 5 - 4, textFontSize);
    tft.drawCentreString("%", tft.width() / 2 + 14, temperatureY, textFontSize);
  */

  tft.setTextColor(COLOR_WARNING_TEXT);
  drawStop();
  state_blink = 1;
  while (!digitalRead(pulse));
  turnFansOn();
  if (temperatureAtStart > temperature[cornerNTC]) {
    temperatureAtStart = temperature[cornerNTC];
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
        basicTemperatureControl();
      }
    }
    if (controlHumidity) {
      basicHumidityControl();
    }
    while (!digitalRead(pulse)) {
      updateData();
      if (digitalRead(pulse) == 0) {
        back_mode();
      }
    }
    blinkGoBackMessage();
  }
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

void basicTemperatureControl() {
  if (temperature[cornerNTC] < desiredIn3Temp) {
    heatUp();
  }
  else {
    analogWrite(HEATER, 0);
  }
}

void basicHumidityControl() {
  if (humidity < desiredIn3Hum) {
    digitalWrite(HUMIDIFIER, HIGH);
  }
  else {
    digitalWrite(HUMIDIFIER, LOW);
  }
}

void heatUp() {
  if (temperature[heaterNTC] < heaterLimitTemp) {
    analogWrite(HEATER, 200);
  }
  else {
    analogWrite(HEATER, 0);
  }
}

