void actuatorsProgress() {
  if (page == mainMenuPage) {
    GPRSSetPostVariables(actuatorsModeON, "ON, wks:" + String(gestationWeeks) + "," + String (desiredSkinTemp, 1) + "," + String (desiredRoomHum));
  }
  else {
    GPRSSetPostVariables(actuatorsModeON, "ON:" + String (desiredSkinTemp, 1) + "," + String (desiredRoomHum));
  }
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

  setGPRSPostPeriod(actuatingGPRSPostPeriod);
  if (controlTemperature) {
    temperatureAtStart = temperature[babyNTC];
    printLoadingTemperatureBar();
    switch (language) {
      case spanish:
      case portuguese:
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
      case portuguese:
        textToWrite = "Umidade";
        break;

    }
    tft.drawCentreString(textToWrite, tft.width() / 2, humBarPosY - 4 * letter_height / 3, textFontSize);
  }
  tft.setTextColor(COLOR_WARNING_TEXT);
  drawStop();
  state_blink = 1;
  while (!digitalRead(ENC_SWITCH)) {
    updateData();
  }
  turnFans(ON);
  if (temperatureAtStart > temperature[babyNTC]) {
    temperatureAtStart = temperature[babyNTC];
  }
  while (1) {
    updateData();
    if (controlTemperature) {
      basictemperatureControl();
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
    pwmWrite(HEATER, LOW);
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
  pwmWrite(HEATER, heaterMaxPWM * maxHeaterPower / 100 );
}

void turnActuators(bool mode) {
  pwmWrite(HEATER, mode * heaterMaxPWM * maxHeaterPower / 100 );
  digitalWrite(HUMIDIFIER, mode);
}

void turnFans(bool mode) {
  digitalWrite(FAN, mode);
}
