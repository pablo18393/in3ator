/*
  MIT License

  Copyright (c) 2022 Medical Open World, Pablo Sánchez Bergasa

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

*/

void UI_actuatorsProgress() {
  bool exitActuation = false;
  alarmTimerStart();
  if (page == errorPage) {
    GPRSSetPostVariables(NO_COMMENT, "ALARM");
  }
  else {
    GPRSSetPostVariables(actuatorsModeON, "ON:" + String (desiredControlTemperature, 1) + "," + String (desiredControlHumidity));
  }
  byte  numWords = false;
  temperaturePercentage = false;
  page = actuatorsProgressPage;
  tft.setTextSize(1);
  print_text = false;
  menu_rows = numWords;
  graphics();
  drawHeading();
  setTextColor(COLOR_MENU_TEXT);
  setSensorsGraphicPosition(page);
  drawActuatorsSeparators();

  setGPRSPostPeriod(actuatingGPRSPostPeriod);

  if (controlMode) {
    switch (language) {
      case spanish:
        textToWrite = convertStringToChar(cstring, "Temperatura aire");
        break;
      case portuguese:
        textToWrite = convertStringToChar(cstring, "Temperatura do ar");
        break;
      case english:
        textToWrite = convertStringToChar(cstring, "Air temperature");
        break;
      case french:
        textToWrite = convertStringToChar(cstring, "Temperature de l'air");
        break;
    }
  }
  else {
    switch (language) {
      case spanish:
        textToWrite = convertStringToChar(cstring, "Temperatura piel");
        break;
      case portuguese:
        textToWrite = convertStringToChar(cstring, "temperatura da pele");
        break;
      case english:
        textToWrite = convertStringToChar(cstring, "Skin temperature");
        break;
      case french:
        textToWrite = convertStringToChar(cstring, "Temperature de la peau");
        break;
    }
  }
  drawCentreString(textToWrite, tft.width() / 2, tempBarPosY - 4 * letter_height / 3, textFontSize);
  if (!controlMode) {
    switch (language) {
      case spanish:
        textToWrite = convertStringToChar(cstring, "Temperatura aire");
        break;
      case portuguese:
        textToWrite = convertStringToChar(cstring, "Temperatura do ar");
        break;
      case english:
        textToWrite = convertStringToChar(cstring, "Air temperature");
        break;
      case french:
        textToWrite = convertStringToChar(cstring, "Temperature de l'air");
        break;
    }
  }
  else {
    switch (language) {
      case spanish:
        textToWrite = convertStringToChar(cstring, "Temperatura piel");
        break;
      case portuguese:
        textToWrite = convertStringToChar(cstring, "temperatura da pele");
        break;
      case english:
        textToWrite = convertStringToChar(cstring, "Skin temperature");
        break;
      case french:
        textToWrite = convertStringToChar(cstring, "Temperature de la peau");
        break;
    }
  }
  drawCentreString(textToWrite, tft.width() / 2, tft.height() / 2 - letter_height, textFontSize);

  switch (language) {
    case spanish:
      textToWrite = convertStringToChar(cstring, "Humedad");
      break;
    case english:
      textToWrite = convertStringToChar(cstring, "Humidity");
      break;
    case french:
      textToWrite = convertStringToChar(cstring, "Humidite");
      break;
    case portuguese:
      textToWrite = convertStringToChar(cstring, "Umidade");
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
  if (temperatureToControl < desiredControlTemperature) {
    heatUp();
  }
  else {
    ledcWrite(HEATER_PWM_CHANNEL, LOW);
  }
}

void basicHumidityControl() {
  if (humidity < desiredControlHumidity) {
    if (!humidifierState || humidifierStateChange) {
in3_hum.turn(ON);
      humidifierStateChange = false;
    }
    humidifierState = true;
  }
  else {
    if (humidifierState || humidifierStateChange) {
in3_hum.turn(OFF);
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
  if(mode * ongoingCriticalAlarm()){
    in3_hum.turn(ON);
  }
  else{
    in3_hum.turn(OFF);
  }
  turnFans(mode);
}

void turnFans(bool mode) {
  GPIOWrite(FAN, mode);
}
