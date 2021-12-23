void actuatorsProgress() {
  bool exitActuation = false;
  if (page == errorPage) {
    GPRSSetPostVariables(NULL, "ALARM");
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
  if (controlMode) {
    temperatureAtStart = temperature[airNTC];
  }
  else {
    temperatureAtStart = temperature[babyNTC];
  }
  if (controlMode) {
    switch (language) {
      case spanish:
        textToWrite = "Temperatura aire";
        break;
      case portuguese:
        textToWrite = "Temperatura do ar";
        break;
      case english:
        textToWrite = "Air temperature";
        break;
      case french:
        textToWrite = "Temperature de l'air";
        break;
    }
  }
  else {
    switch (language) {
      case spanish:
        textToWrite = "Temperatura piel";
        break;
      case portuguese:
        textToWrite = "temperatura da pele";
        break;
      case english:
        textToWrite = "Skin temperature";
        break;
      case french:
        textToWrite = "Temperature de la peau";
        break;
    }
  }
  drawCentreString(textToWrite, tft.width() / 2, tempBarPosY - 4 * letter_height / 3, textFontSize);
  if (!controlMode) {
    switch (language) {
      case spanish:
        textToWrite = "Temperatura aire";
        break;
      case portuguese:
        textToWrite = "Temperatura do ar";
        break;
      case english:
        textToWrite = "Air temperature";
        break;
      case french:
        textToWrite = "Temperature de l'air";
        break;
    }
  }
  else {
    switch (language) {
      case spanish:
        textToWrite = "Temperatura piel";
        break;
      case portuguese:
        textToWrite = "temperatura da pele";
        break;
      case english:
        textToWrite = "Skin temperature";
        break;
      case french:
        textToWrite = "Temperature de la peau";
        break;
    }
  }
  drawCentreString(textToWrite, tft.width() / 2, tft.height() / 2 - letter_height, textFontSize);
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
  if (controlAlgorithm == PID_CONTROL) {
    if (controlTemperature) {
      startTemperaturePID();
    }
    if (controlHumidity) {
      startHumidityPID();
    }
  }
  updateDisplaySensors();
  alarmTimerStart();

  while (!exitActuation) {
    updateData();
    if (controlTemperature) {
      if (controlMode) {
        alarmSensedValue = temperature[airNTC];
      }
      else {
        alarmSensedValue = temperature[babyNTC];
      }
      if (checkAlarms(desiredSkinTemp, alarmSensedValue, temperatureError, temperatureAlarmTime)) {
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
      if (controlAlgorithm == BASIC_CONTROL) {
        basictemperatureControl();
      }
      else {
        PIDHandler();
      }
    }
    if (controlHumidity) {
      checkTCAHealth();
      checkTFTHealth();
      if (controlAlgorithm == BASIC_CONTROL) {
        basicHumidityControl();
      }
      else {
        PIDHandler();
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
      exitActuation = back_mode();
    }
    blinkGoBackMessage();
  }
}


void basictemperatureControl() {
  float temperatureToControl;
  if (controlMode) {
    temperatureToControl = temperature[airNTC];
  }
  else {
    temperatureToControl = temperature[babyNTC];
  }
  if (temperatureToControl < desiredSkinTemp) {
    heatUp();
  }
  else {
    ledcWrite(HEATER_PWM_CHANNEL, LOW);
  }
}

void basicHumidityControl() {
  /*
    if (humidity < desiredRoomHum) {
    GPIOWrite(HUMIDIFIER, HIGH);
    }
    else {
    GPIOWrite(HUMIDIFIER, LOW);
    }
  */

  if (humidity < desiredRoomHum) {
    if (!humidifierState || humidifierStateChange) {
      GPIOWrite(HUMIDIFIER, HIGH);
      humidifierStateChange = false;
    }
    humidifierState = true;
  }
  else {
    if (humidifierState || humidifierStateChange) {
      GPIOWrite(HUMIDIFIER, LOW);
      humidifierStateChange = false;
    }
    humidifierState = false;
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
