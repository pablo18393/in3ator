
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
      words[temperatureCalibrationGraphicPosition]  = "First point";
      words[setCalibrationGraphicPosition]  = "SET";
      break;
    case spanish:
      words[temperatureCalibrationGraphicPosition]  = "First point";
      words[setCalibrationGraphicPosition]  = "SET";
      break;
    case french:
      words[temperatureCalibrationGraphicPosition]  = "First point";
      words[setCalibrationGraphicPosition]  = "SET";
      break;
    case portuguese:
      words[temperatureCalibrationGraphicPosition]  = "First point";
      words[setCalibrationGraphicPosition]  = "SET";
      break;
  }
  rectangles = numWords;
  graphics();
  drawHeading();
  bar_pos = true;
  ypos = graphicHeight(bar_pos - 1);
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
      words[temperatureCalibrationGraphicPosition]  = "Second point";
      words[setCalibrationGraphicPosition]  = "SET";
      break;
    case spanish:
      words[temperatureCalibrationGraphicPosition]  = "Second point";
      words[setCalibrationGraphicPosition]  = "SET";
      break;
    case french:
      words[temperatureCalibrationGraphicPosition]  = "Second point";
      words[setCalibrationGraphicPosition]  = "SET";
      break;
    case portuguese:
      words[temperatureCalibrationGraphicPosition]  = "Second point";
      words[setCalibrationGraphicPosition]  = "SET";
      break;
  }
  rectangles = numWords;
  graphics();
  drawHeading();
  bar_pos = true;
  ypos = graphicHeight(bar_pos - 1);
  while (!GPIORead(ENC_SWITCH)) {
    updateData();
  }
  vTaskDelay(debounceTime);
}

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
      words[autoCalibrationMessageGraphicPosition]  = "Calibrating...";
      break;
    case spanish:
      words[autoCalibrationMessageGraphicPosition]  = "Calibrating...";
      break;
    case french:
      words[autoCalibrationMessageGraphicPosition]  = "Calibrating...";
      break;
    case portuguese:
      words[autoCalibrationMessageGraphicPosition]  = "Calibrating...";
      break;
  }
  rectangles = numWords;
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
