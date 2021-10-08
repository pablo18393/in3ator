void actuatorsProgress() {
  if (page == errorPage) {
    GPRSSetPostVariables(NULL, "ALARM");
  }
  else if (page == mainMenuPage) {
    GPRSSetPostVariables(actuatorsModeON, "ON, wks:" + String(gestationWeeks) + "," + String (desiredSkinTemp, 1) + "," + String (desiredRoomHum));
  }
  else {
    GPRSSetPostVariables(actuatorsModeON, "ON:" + String (desiredSkinTemp, 1) + "," + String (desiredRoomHum));
  }
  byte  numWords = false;
  temperaturePercentage = false;
  page = actuatorsProgressPage;
  tft.setTextSize(1);
  print_text = false;
  rectangles = numWords;
  graphics();
  drawHeading();
  setTextColor(COLOR_MENU_TEXT);
  setSensorsGraphicPosition();
  drawActuatorsSeparators();

  setGPRSPostPeriod(actuatingGPRSPostPeriod);
  if (controlTemperature) {
    printLoadingTemperatureBar();
  }
  temperatureAtStart = temperature[babyNTC];
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
  drawCentreString(textToWrite, tft.width() / 2, tempBarPosY - 4 * letter_height / 3, textFontSize);
  if (controlHumidity) {
    printLoadingHumidityBar();
  }
  humidityAtStart = humidity;
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
  drawCentreString(textToWrite, tft.width() / 2, humBarPosY - 4 * letter_height / 3, textFontSize);
  setTextColor(COLOR_WARNING_TEXT);
  drawStop();
  state_blink = true;
  while (!GPIORead(ENC_SWITCH)) {
    updateData();
  }
  turnFans(ON);
  if (temperatureAtStart > temperature[babyNTC]) {
    temperatureAtStart = temperature[babyNTC];
  }
  if (controlMode == PID_CONTROL) {
    if (controlTemperature) {
      startTemperaturePID();
    }
    if (controlHumidity) {
      startHumidityPID();
    }
  }
  alarmTimerStart();
  while (1) {
    updateData();
    if (controlTemperature) {
      if (checkAlarms(desiredSkinTemp, temperature[babyNTC], temperatureError, temperatureAlarmTime)) {
        if (!alarmOnGoing[temperatureAlarm]) {
          alarmOnGoing[temperatureAlarm] = true;
          buzzerConstantTone(buzzerAlarmTone);
          drawAlarmMessage(DRAW, temperatureAlarm);
        }
      }
      else {
        if (alarmOnGoing[temperatureAlarm]) {
          alarmOnGoing[temperatureAlarm] = false;
          shutBuzzer();
          drawAlarmMessage(ERASE, temperatureAlarm);
        }
      }
      if (controlMode == BASIC_CONTROL) {
        basictemperatureControl();
      }
    }
    if (controlHumidity) {
      if (controlMode == BASIC_CONTROL) {
        basicHumidityControl();
      }
      if (checkAlarms(humidity, desiredRoomHum, humidityError, humidityAlarmTime)) {
        if (!alarmOnGoing[humidityAlarm]) {
          alarmOnGoing[humidityAlarm] = true;
          buzzerConstantTone(buzzerAlarmTone);
          drawAlarmMessage(DRAW, humidityAlarm);
        }
      }
      else {
        if (alarmOnGoing[humidityAlarm]) {
          alarmOnGoing[humidityAlarm] = false;
          shutBuzzer();
          drawAlarmMessage(ERASE, humidityAlarm);
        }
      }
    }
    if (!alarmOnGoing[temperatureAlarm] && !alarmOnGoing[humidityAlarm]) {
      if (millis() - buzzerStandbyTime > buzzerStandbyPeriod) {
        buzzerStandbyTime = millis();
        //buzzerTone(buzzerStandbyToneTimes, buzzerStandbyToneDuration, buzzerStandbyTone);
      }
    }
    while (!GPIORead(ENC_SWITCH)) {
      updateData();
      back_mode();
    }
    blinkGoBackMessage();
  }
}


void basictemperatureControl() {
  if (temperature[babyNTC] < desiredSkinTemp) {
    heatUp();
  }
  else {
    ledcWrite(HEATER_PWM_CHANNEL, LOW);
  }
}

void basicHumidityControl() {
  if (humidity < desiredRoomHum) {
    GPIOWrite(HUMIDIFIER, HIGH);
  }
  else {
    GPIOWrite(HUMIDIFIER, LOW);
  }
}

void heatUp() {
  ledcWrite(HEATER_PWM_CHANNEL, heaterMaxPWM * HeaterPower / 100);
}

void turnActuators(bool mode) {
  ledcWrite(HEATER_PWM_CHANNEL, mode * heaterMaxPWM * HeaterPower / 100 );
  GPIOWrite(HUMIDIFIER, mode);
}

void turnFans(bool mode) {
  GPIOWrite(FAN, mode);
}
