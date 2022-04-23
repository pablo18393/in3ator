void actuatorsProgress() {
  bool exitActuation = false;
  alarmTimerStart();
  if (page == errorPage) {
    GPRSSetPostVariables(NULL, "ALARM");
  }
  else {
    GPRSSetPostVariables(actuatorsModeON, "ON:" + String (desiredControlTemp, 1) + "," + String (desiredRoomHum));
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
  if (controlAlgorithm == PID_CONTROL) {
    if (controlTemperature) {
      switch (controlMode) {
        case SKIN_CONTROL:
          startPID(skinPID);
          break;
        case AIR_CONTROL:
          startPID(airPID);
          break;
      }
    }
    if (controlHumidity) {
      startPID(humidityPID);
    }
  }
  updateDisplaySensors();
  if (controlTemperature) {
    printLoadingTemperatureBar();
    if (controlMode) {
      temperatureAtStart = temperature[airSensor];
    }
    else {
      temperatureAtStart = temperature[skinSensor];
    }
    exitActuation = !checkFan();
  }
  if (controlHumidity) {
    printLoadingHumidityBar();
  }
  humidityAtStart = humidity;
  turnFans(ON);
  while (!exitActuation) {
    updateData();
    if (controlTemperature) {
      if (controlAlgorithm == BASIC_CONTROL) {
        basictemperatureControl();
      }
      else {
        PIDHandler();
      }
    }
    if (controlHumidity) {
      if (controlAlgorithm == BASIC_CONTROL) {
        basicHumidityControl();
      }
      else {
        PIDHandler();
      }
    }
    while (!GPIORead(ENC_SWITCH)) {
      updateData();
      exitActuation = back_mode();
    }
    blinkGoBackMessage();
  }
  stopActuation();
}


void basictemperatureControl() {
  float temperatureToControl;
  if (controlMode) {
    temperatureToControl = temperature[airSensor];
  }
  else {
    temperatureToControl = temperature[skinSensor];
  }
  if (temperatureToControl < desiredControlTemp) {
    heatUp();
  }
  else {
    ledcWrite(HEATER_PWM_CHANNEL, LOW);
  }
}

void basicHumidityControl() {
  if (humidity < desiredRoomHum) {
    if (!humidifierState || humidifierStateChange) {
      if (HUMIDIFIER_MODE == HUMIDIFIER_PWM) {
        ledcWrite(HUMIDIFIER_PWM_CHANNEL, HUMIDIFIER_DUTY_CYCLE * ongoingCriticalAlarm());
      }
      else {
        GPIOWrite(HUMIDIFIER, HIGH);
      }
      humidifierStateChange = false;
    }
    humidifierState = true;
  }
  else {
    if (humidifierState || humidifierStateChange) {
      if (HUMIDIFIER_MODE == HUMIDIFIER_PWM) {
        ledcWrite(HUMIDIFIER_PWM_CHANNEL, false);
      }
      else {
        GPIOWrite(HUMIDIFIER, LOW);
      }
      humidifierStateChange = false;
    }
    humidifierState = false;
  }
}


void heatUp() {
  ledcWrite(HEATER_PWM_CHANNEL, BUZZER_MAX_PWR * ongoingCriticalAlarm());
}

void stopActuation() {
  stopPID(airPID);
  stopPID(skinPID);
  stopPID(humidityPID);
  turnActuators(OFF);
}

void turnActuators(bool mode) {
  ledcWrite(HEATER_PWM_CHANNEL, mode * BUZZER_MAX_PWR * ongoingCriticalAlarm());
  if (HUMIDIFIER_MODE == HUMIDIFIER_PWM) {
    ledcWrite(HUMIDIFIER_PWM_CHANNEL, HUMIDIFIER_DUTY_CYCLE * mode * ongoingCriticalAlarm());
  }
  else {
    GPIOWrite(HUMIDIFIER, mode);
  }
  turnFans(mode);
}

void turnFans(bool mode) {
  GPIOWrite(FAN, mode);
}
