
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
  delay(debounceTime);
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
  delay(debounceTime);
}

void autoCalibration() {
  bool exitCalibrationMenu = false;
  byte numWords = 1;
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
  delay(debounceTime);
  autoCalibrationProcess = setupAutoCalibrationPoint;
  while (!exitCalibrationMenu) {
    updateData();
    switch (autoCalibrationProcess) {
      case setupAutoCalibrationPoint:
        Serial.println("point 0");
        clearCalibrationValues();
        autoCalibrationProcess = firstAutoCalibrationPoint;
        break;
      case firstAutoCalibrationPoint:
        if (!GPIORead(ENC_SWITCH)) {
          provisionalReferenceTemperatureLow = temperature[airSensor];
          provisionalRawTemperatureLow[babySensor] = temperature[babySensor];
          delay(debounceTime);
          while (!GPIORead(ENC_SWITCH)) {
            updateData();
            exitCalibrationMenu = back_mode();
          }
          delay(debounceTime);
          ledcWrite(HEATER_PWM_CHANNEL, PWM_MAX_VALUE / 2);
          turnFans(ON);
          autoCalibrationProcess = secondAutoCalibrationPoint;
          Serial.println("point 1");
        }
        break;
      case secondAutoCalibrationPoint:
        if (!GPIORead(ENC_SWITCH)) {
          Serial.println("point 2");
          ReferenceTemperatureLow = provisionalReferenceTemperatureLow;
          RawTemperatureLow[babySensor] = provisionalRawTemperatureLow[babySensor];
          ReferenceTemperatureRange = temperature[airSensor] - ReferenceTemperatureLow;
          RawTemperatureRange[babySensor] = (temperature[babySensor] - RawTemperatureLow[babySensor]);
          logln("calibration factors: " + String(RawTemperatureLow [babySensor]) + "," + String (RawTemperatureRange [babySensor]) + "," + String (ReferenceTemperatureRange) + "," + String (ReferenceTemperatureLow));
          saveCalibrationToEEPROM();
          ledcWrite(HEATER_PWM_CHANNEL, false);
          turnFans(OFF);
          exitCalibrationMenu=true;
        }
        break;
    }
  }
  mainMenu();
}

void clearCalibrationValues() {
  RawTemperatureLow[babySensor] = false;
  RawTemperatureRange[babySensor] = false;
  RawTemperatureLow[airSensor] = false;
  RawTemperatureRange[airSensor] = false;
  ReferenceTemperatureRange = false;
  ReferenceTemperatureLow = false;
}
