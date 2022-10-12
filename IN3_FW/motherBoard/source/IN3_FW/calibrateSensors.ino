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

void autoCalibration() {
  bool exitCalibrationMenu = false;
  byte numWords = 1;
  long  lastTemperatureMeasurement = millis();
  float timeBetweenMeasurements = 4; //in minutes
  double stabilityError = 0.1; //ºC
  int historyLength = 5;
  int historyLengthPosition = false;
  double referenceSensorHistory[historyLength];
  double sensorToCalibrateHistory[historyLength];
  referenceSensorHistory[0] = temperature[airSensor];
  sensorToCalibrateHistory[0] = temperature[skinSensor];
  page = autoCalibrationPage;
  print_text = true;
  tft.setTextSize(1);
  setTextColor(COLOR_MENU_TEXT);
  for (int i = false; i < numWords; i++) {
    pos_text[i] = LEFT_MARGIN;
  }
  pos_text[setCalibrationGraphicPosition] = CENTER;
  switch (language) {
    case english:
      words[autoCalibrationMessageGraphicPosition]  = convertStringToChar("Calibrating...");
      break;
    case spanish:
      words[autoCalibrationMessageGraphicPosition]  = convertStringToChar("Calibrating...");
      break;
    case french:
      words[autoCalibrationMessageGraphicPosition]  = convertStringToChar("Calibrating...");
      break;
    case portuguese:
      words[autoCalibrationMessageGraphicPosition]  = convertStringToChar("Calibrating...");
      break;
  }
  menu_rows = numWords;
  graphics();
  drawHeading();
  bar_pos = true;
  ypos = graphicHeight(bar_pos - 1);
  while (!GPIORead(ENC_SWITCH)) {
    updateData();
  }
  vTaskDelay(debounceTime);
  autoCalibrationProcess = setupAutoCalibrationPoint;
  while (!exitCalibrationMenu) {
    updateData();
    switch (autoCalibrationProcess) {
      case setupAutoCalibrationPoint:
        Serial.println("=================================================point 0");
        clearCalibrationValues();
        autoCalibrationProcess = firstAutoCalibrationPoint;
        turnFans(ON);
        break;
      case firstAutoCalibrationPoint:
        if (!GPIORead(ENC_SWITCH) || checkStableTemperatures(referenceSensorHistory, sensorToCalibrateHistory, historyLength, stabilityError)) {
          provisionalReferenceTemperatureLow = temperature[airSensor];
          provisionalRawTemperatureLow[skinSensor] = temperature[skinSensor];
          vTaskDelay(debounceTime);
          while (!GPIORead(ENC_SWITCH)) {
            updateData();
            exitCalibrationMenu = back_mode();
          }
          vTaskDelay(debounceTime);
          desiredControlTemperature = 36;
          startPID(airPID);
          //ledcWrite(HEATER_PWM_CHANNEL, HEATER_HALF_PWR * ongoingCriticalAlarm());
          autoCalibrationProcess = secondAutoCalibrationPoint;
          referenceSensorHistory[historyLengthPosition] = false;
          sensorToCalibrateHistory[historyLengthPosition] = false;
          Serial.println("=================================================point 1");
        }
        break;
      case secondAutoCalibrationPoint:
        PIDHandler();
        if (!GPIORead(ENC_SWITCH) || checkStableTemperatures(referenceSensorHistory, sensorToCalibrateHistory, historyLength, stabilityError)) {
          Serial.println("=================================================point 2");
          ReferenceTemperatureLow = provisionalReferenceTemperatureLow;
          RawTemperatureLow[skinSensor] = provisionalRawTemperatureLow[skinSensor];
          ReferenceTemperatureRange = temperature[airSensor] - ReferenceTemperatureLow;
          RawTemperatureRange[skinSensor] = (temperature[skinSensor] - RawTemperatureLow[skinSensor]);
          logln("calibration factors: " + String(RawTemperatureLow [skinSensor]) + "," + String (RawTemperatureRange [skinSensor]) + "," + String (ReferenceTemperatureRange) + "," + String (ReferenceTemperatureLow));
          saveCalibrationToEEPROM();
          ledcWrite(HEATER_PWM_CHANNEL, false);
          turnFans(OFF);
          exitCalibrationMenu = true;
          stopPID(airPID);
        }
        break;
    }
    if (millis() - lastTemperatureMeasurement > minsToMillis(timeBetweenMeasurements)) {
      lastTemperatureMeasurement = millis();
      if (historyLengthPosition == historyLength) {
        historyLengthPosition = false;
      }
      referenceSensorHistory[historyLengthPosition] = temperature[airSensor];
      sensorToCalibrateHistory[historyLengthPosition] = temperature[skinSensor];
      historyLengthPosition++;

      for (int i = 0; i < historyLength; i++) {
        Serial.println (abs(*referenceSensorHistory  - * (referenceSensorHistory + i)));
        Serial.println (abs(*sensorToCalibrateHistory  - * (sensorToCalibrateHistory + i)));
      }
    }
  }
  UI_mainMenu();
}

void fineTuneCalibration() {
  byte numWords = 2;
  fineTuneSkinTemperature = false;
  page = fineTuneCalibrationPage;
  print_text = true;
  tft.setTextSize(1);
  setTextColor(COLOR_MENU_TEXT);
  for (int i = false; i < numWords; i++) {
    pos_text[i] = LEFT_MARGIN;
  }
  pos_text[setCalibrationGraphicPosition] = CENTER;
  words[temperatureCalibrationGraphicPosition]  = convertStringToChar("Temperature adjust");
  words[setCalibrationGraphicPosition]  = convertStringToChar("SET");
  menu_rows = numWords;
  graphics();
  drawHeading();
  bar_pos = true;
  ypos = graphicHeight(bar_pos - 1);
  setTextColor(COLOR_MENU_TEXT);
  drawFloat(temperature[skinSensor], 1, valuePosition, ypos, textFontSize);
  while (!GPIORead(ENC_SWITCH)) {
    updateData();
  }
  vTaskDelay(debounceTime);
}

void firstPointCalibration() {
  byte numWords = 2;
  page = firstPointCalibrationPage;
  print_text = true;
  tft.setTextSize(1);
  setTextColor(COLOR_MENU_TEXT);
  for (int i = false; i < numWords; i++) {
    pos_text[i] = LEFT_MARGIN;
  }
  pos_text[setCalibrationGraphicPosition] = CENTER;
  switch (language) {
    case english:
      words[temperatureCalibrationGraphicPosition]  = convertStringToChar("First point");
      words[setCalibrationGraphicPosition]  = convertStringToChar("SET");
      break;
    case spanish:
      words[temperatureCalibrationGraphicPosition]  = convertStringToChar("First point");
      words[setCalibrationGraphicPosition]  = convertStringToChar("SET");
      break;
    case french:
      words[temperatureCalibrationGraphicPosition]  = convertStringToChar("First point");
      words[setCalibrationGraphicPosition]  = convertStringToChar("SET");
      break;
    case portuguese:
      words[temperatureCalibrationGraphicPosition]  = convertStringToChar("First point");
      words[setCalibrationGraphicPosition]  = convertStringToChar("SET");
      break;
  }
  menu_rows = numWords;
  graphics();
  drawHeading();
  bar_pos = true;
  ypos = graphicHeight(bar_pos - 1);
  setTextColor(COLOR_MENU_TEXT);
  drawFloat(temperature[skinSensor], 1, valuePosition, ypos, textFontSize);
  while (!GPIORead(ENC_SWITCH)) {
    updateData();
  }
  vTaskDelay(debounceTime);
}

void secondPointCalibration() {
  byte numWords = 2;
  page = secondPointCalibrationPage;
  print_text = true;
  tft.setTextSize(1);
  setTextColor(COLOR_MENU_TEXT);
  for (int i = false; i < numWords; i++) {
    pos_text[i] = LEFT_MARGIN;
  }
  pos_text[setCalibrationGraphicPosition] = CENTER;
  switch (language) {
    case english:
      words[temperatureCalibrationGraphicPosition]  = convertStringToChar("Second point");
      words[setCalibrationGraphicPosition]  = convertStringToChar("SET");
      break;
    case spanish:
      words[temperatureCalibrationGraphicPosition]  = convertStringToChar("Second point");
      words[setCalibrationGraphicPosition]  = convertStringToChar("SET");
      break;
    case french:
      words[temperatureCalibrationGraphicPosition]  = convertStringToChar("Second point");
      words[setCalibrationGraphicPosition]  = convertStringToChar("SET");
      break;
    case portuguese:
      words[temperatureCalibrationGraphicPosition]  = convertStringToChar("Second point");
      words[setCalibrationGraphicPosition]  = convertStringToChar("SET");
      break;
  }
  menu_rows = numWords;
  graphics();
  drawHeading();
  bar_pos = true;
  ypos = graphicHeight(bar_pos - 1);
  setTextColor(COLOR_MENU_TEXT);
  drawFloat(temperature[skinSensor], 1, valuePosition, ypos, textFontSize);
  while (!GPIORead(ENC_SWITCH)) {
    updateData();
  }
  vTaskDelay(debounceTime);
}

bool checkStableTemperatures(double *referenceSensorHistory, double *sensorToCalibrateHistory, int historyLength, double stabilityError) {
  for (int i = 0; i < historyLength; i++) {
    if (abs(*referenceSensorHistory  - * (referenceSensorHistory + i)) > stabilityError) {
      return false;
    }
    if (abs(*sensorToCalibrateHistory - * (sensorToCalibrateHistory + i)) > stabilityError) {
      return false;
    }
  }
  return true;
}

bool checkStableCurrentConsumption(double *referenceSensorHistory, int historyLength, double stabilityError) {
  for (int i = 0; i < historyLength; i++) {
    if (abs(*referenceSensorHistory  - * (referenceSensorHistory + i)) > stabilityError) {
      return false;
    }
  }
  return true;
}

void clearCalibrationValues() {
  RawTemperatureLow[skinSensor] = false;
  RawTemperatureRange[skinSensor] = false;
  RawTemperatureLow[airSensor] = false;
  RawTemperatureRange[airSensor] = false;
  ReferenceTemperatureRange = false;
  ReferenceTemperatureLow = false;
}
