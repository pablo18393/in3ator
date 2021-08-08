
void barSelection() {
  bar_pos = true;
  selected = false;
  ypos = (tft.height() - height_heading) / (2 * rectangles) + letter_height;
  if (page == askSuccessPage) {
    eraseBar();
    bar_pos++;
    updateBar();
  }
  while (1) {
    updateData();
    if (EncMove) {
      if (!selected) {
        if (EncMove < 0) {
          EncMove++;
          if (page == mainMenuPage || page == advancedModePage) {
            enableSetProcess = enableSet;
          }
          else {
            enableSetProcess = true;
          }
          if (bar_pos < rectangles - !enableSetProcess) {
            eraseBar();
            bar_pos++;
            updateBar();
          }
        }
        else {
          EncMove --;
          if (bar_pos > (1 + checkAskSuccessPage())) {
            eraseBar();
            bar_pos--;
            updateBar();
          }
        }
        ypos = graphicHeight(bar_pos - 1);
      }
    }
    if (!digitalRead(ENC_SWITCH)) {
      selected = ! selected;
      if (selected) {
        tft.fillRect(0, (tft.height() - height_heading) * (bar_pos - 1) / rectangles + height_heading, width_select, (tft.height() - height_heading) / rectangles, COLOR_CHOSEN);
      }
      else {
        tft.fillRect(0, (tft.height() - height_heading) * (bar_pos - 1) / rectangles + height_heading, width_select, (tft.height() - height_heading) / rectangles, WHITE);
      }
      for (int i = 2; i <= rectangles; i++) {
        tft.fillRect(0, (tft.height() - height_heading) * (i - 1) / rectangles + height_heading - 1, tft.height(), width_indentation, WHITE); //mejorable
      }
      while (!digitalRead(ENC_SWITCH)) {
        updateData();
        checkEncoderPress();
        if (page != mainMenuPage && page != askSuccessPage) {
          back_mode();
        }
      }
      delay(debounceTime);
      switch (page) {
        case mainMenuPage:
          switch (bar_pos - graphicTextOffset ) {
            case gestationGraphicPosition:
              while (digitalRead(ENC_SWITCH)) {
                updateData();
                if (EncMove && -EncMove + gestationWeeks >= minGestation && -EncMove + gestationWeeks <= maxGestation) {
                  tft.setTextColor(COLOR_MENU);
                  controlTemperature = true;
                  controlHumidity = true;
                  tft.drawRightString(initialSensorsValue, gestationWeeksXPos, ypos, textFontSize);
                  drawRightNumber(gestationWeeks, gestationWeeksXPos, ypos);
                  gestationWeeks -= EncMove;
                  setPresetRoomValues();
                  tft.setTextColor(COLOR_MENU_TEXT);
                  drawRightNumber(gestationWeeks, gestationWeeksXPos, ypos);
                  enableSet = true;
                }
                EncMove = false;
              }
              drawStartMessage();
              break;
            case advancedModeGraphicPosition:
              advancedMode();
              break;
            case LEDGraphicPosition:
                  jaundiceEnable = !jaundiceEnable;
                  tft.setTextColor(COLOR_MENU);
                  if (jaundiceEnable) {
                    tft.drawRightString("OFF", unitPosition, ypos, textFontSize);
                  }
                  else {
                    tft.drawRightString("ON", unitPosition, ypos, textFontSize);
                  }
                  tft.setTextColor(COLOR_MENU_TEXT);
                  if (jaundiceEnable) {
                    tft.drawRightString("ON", unitPosition, ypos, textFontSize);
                    GPRSSetPostVariables(jaundiceLEDON, "");
                    setGPRSPostPeriod(jaundiceGPRSPostPeriod);
                  }
                  else {
                    tft.drawRightString("OFF", unitPosition, ypos, textFontSize);
                    GPRSSetPostVariables(jaundiceLEDOFF, "");
                    GPRSSetPostVariables(jaundiceLEDOFF, "");
                    setGPRSPostPeriod(standByGPRSPostPeriod);
                  }
                  digitalWrite(PHOTOTHERAPY, jaundiceEnable);
              break;
            case settingsGraphicPosition:
              settings();
              break;
            case startGraphicPosition:
              actuatorsProgress();
              break;
          }
          break;
        case advancedModePage:
          switch (bar_pos - graphicTextOffset ) {
            case temperatureGraphicPosition:
              while (digitalRead(ENC_SWITCH)) {
                updateData();
                if (EncMove && -EncMove + desiredSkinTemp >= minTemp && -EncMove + desiredSkinTemp <= maxTemp) {
                  tft.setTextColor(COLOR_MENU);
                  if (!controlTemperature) {
                    controlTemperature = true;
                    tft.drawRightString(initialSensorsValue, initialSensorPosition, temperatureY, textFontSize);
                  }
                  tft.drawFloat(desiredSkinTemp, 1, temperatureX - 65, temperatureY, textFontSize);
                  desiredSkinTemp -= float(EncMove) / 10;
                  tft.setTextColor(COLOR_MENU_TEXT);
                  tft.drawFloat(desiredSkinTemp, 1, temperatureX - 65, temperatureY, textFontSize);
                  enableSet = true;
                }
                EncMove = false;
              }
              drawStartMessage();
              break;
            case humidityGraphicPosition:
              while (digitalRead(ENC_SWITCH)) {
                updateData();
                if (EncMove && -EncMove + desiredRoomHum >= minHum && -EncMove + desiredRoomHum <= maxHum) {
                  tft.setTextColor(COLOR_MENU);
                  if (!controlHumidity) {
                    controlHumidity = true;
                    tft.drawRightString(initialSensorsValue, initialSensorPosition, humidityY, textFontSize);
                  }
                  drawCentreNumber(desiredRoomHum, humidityX - 65, humidityY);
                  desiredRoomHum -= (EncMove);
                  tft.setTextColor(COLOR_MENU_TEXT);
                  drawCentreNumber(desiredRoomHum, humidityX - 65, humidityY);
                  enableSet = true;
                }
                EncMove = false;
              }
              drawStartMessage();
              break;
            case LEDGraphicPosition:
              jaundiceEnable = !jaundiceEnable;
              tft.setTextColor(COLOR_MENU);
              if (jaundiceEnable) {
                tft.drawRightString("OFF", unitPosition, ypos, textFontSize);
              }
              else {
                tft.drawRightString("ON", unitPosition, ypos, textFontSize);
              }
              tft.setTextColor(COLOR_MENU_TEXT);
              if (jaundiceEnable) {
                tft.drawRightString("ON", unitPosition, ypos, textFontSize);
                GPRSSetPostVariables(jaundiceLEDON, "");
                setGPRSPostPeriod(jaundiceGPRSPostPeriod);
              }
              else {
                tft.drawRightString("OFF", unitPosition, ypos, textFontSize);
                GPRSSetPostVariables(jaundiceLEDOFF, "");
                GPRSSetPostVariables(jaundiceLEDOFF, "");
                setGPRSPostPeriod(standByGPRSPostPeriod);
              }
              digitalWrite(PHOTOTHERAPY, jaundiceEnable);
              break;
            case settingsGraphicPosition:
              settings();
              break;
            case startGraphicPosition:
              actuatorsProgress();
              break;
          }
          break;
        case askSuccessPage:
          switch (bar_pos - graphicTextOffset ) {
            case afirmativeGraphicPosition:
              GPRSSetPostVariables(actuatorsModeOFF, "Baby survived");
              break;
            case negativeGraphicPosition:
              GPRSSetPostVariables(actuatorsModeOFF, "Baby didn't survive");
              break;
          }
          mainMenu();
          break;
        case settingsPage:
          switch (bar_pos - graphicTextOffset ) {
            case autoLockGraphicPosition:
              tft.setTextColor(COLOR_MENU);
              if (autoLock) {
                switch (language) {
                  case spanish:
                    textToWrite = "SI";
                    break;
                  case english:
                    textToWrite = "YES";
                    break;
                  case french:
                    textToWrite = "OUI";
                    break;
                  case portuguese:
                    textToWrite = "SIM";
                    break;
                }
                tft.drawRightString(textToWrite, unitPosition, ypos, textFontSize);
                tft.setTextColor(COLOR_MENU_TEXT);
                switch (language) {
                  case spanish:
                    textToWrite = "NO";
                    break;
                  case english:
                    textToWrite = "NO";
                    break;
                  case french:
                    textToWrite = "PAS";
                    break;
                  case portuguese:
                    textToWrite = "NAO";
                    break;
                }
                tft.drawRightString(textToWrite, unitPosition, ypos, textFontSize);
              }
              else {
                tft.setTextColor(COLOR_MENU);
                switch (language) {
                  case spanish:
                    textToWrite = "NO";
                    break;
                  case english:
                    textToWrite = "NO";
                    break;
                  case french:
                    textToWrite = "PAS";
                    break;
                  case portuguese:
                    textToWrite = "NAO";
                    break;
                }
                tft.drawRightString(textToWrite, unitPosition, ypos, textFontSize);
                tft.setTextColor(COLOR_MENU_TEXT);
                switch (language) {
                  case spanish:
                    textToWrite = "SI";
                    break;
                  case english:
                    textToWrite = "YES";
                    break;
                  case french:
                    textToWrite = "OUI";
                    break;
                  case portuguese:
                    textToWrite = "SIM";
                    break;
                }
                tft.drawRightString(textToWrite, unitPosition, ypos, textFontSize);
              }
              autoLock = !autoLock;
              EEPROM.write(EEPROM_autoLock, autoLock);
              break;
            case languageGraphicPosition:
              while (digitalRead(ENC_SWITCH)) {
                updateData();
                if (EncMove) {
                  tft.setTextColor(COLOR_MENU);
                  switch (language) {
                    case spanish:
                      textToWrite = "SPA";
                      break;
                    case english:
                      textToWrite = "ENG";
                      break;
                    case french:
                      textToWrite = "FRA";
                      break;
                    case portuguese:
                      textToWrite = "POR";
                      break;
                  }
                  tft.drawRightString(textToWrite, unitPosition, ypos, textFontSize);
                  language -= EncMove;
                  if (language < 0) {
                    language = numLanguages - 1;
                  }
                  if (language >= numLanguages) {
                    language = false;
                  }
                  tft.setTextColor(COLOR_MENU_TEXT);
                  switch (language) {
                    case spanish:
                      textToWrite = "SPA";
                      break;
                    case english:
                      textToWrite = "ENG";
                      break;
                    case french:
                      textToWrite = "FRA";
                      break;
                    case portuguese:
                      textToWrite = "POR";
                      break;
                  }
                  tft.drawRightString(textToWrite, unitPosition, ypos, textFontSize);
                  EEPROM.write(EEPROM_language, language);
                  EncMove = false;
                }
              }
              settings();
              break;
            case controlModeGraphicPosition:
              controlMode = !controlMode;
              EEPROM.write(EEPROM_controlMode, controlMode);
              tft.setTextColor(COLOR_MENU);
              if (controlMode) {
                tft.drawRightString("BASIC", unitPosition, ypos, textFontSize);
              }
              else {
                tft.drawRightString("PID", unitPosition, ypos, textFontSize);
              }
              tft.setTextColor(COLOR_MENU_TEXT);
              if (controlMode) {
                tft.drawRightString("PID", unitPosition, ypos, textFontSize);
              }
              else {
                tft.drawRightString("BASIC", unitPosition, ypos, textFontSize);
              }
              break;
            case heaterPowerGraphicPosition:
              while (digitalRead(ENC_SWITCH) ) {
                updateData();
                if (EncMove && -EncMove + HeaterPower >= heaterPowerMin && -EncMove + HeaterPower <= heaterPowerMax) {
                  tft.setTextColor(COLOR_MENU);
                  drawRightNumber(HeaterPower, 280, ypos);
                  HeaterPower -= 10 * EncMove;
                  EEPROM.write(EEPROM_HeaterPower, HeaterPower);
                  tft.setTextColor(COLOR_MENU_TEXT);
                  drawRightNumber(HeaterPower, 280, ypos);
                }
                EncMove = false;
              }
              break;
            case DebugENGraphicPosition:
              UARTDebug = !UARTDebug;
              EEPROM.write(EEPROM_UARTDebug, UARTDebug);
              tft.setTextColor(COLOR_MENU);
              if (UARTDebug) {
                tft.drawRightString("OFF", unitPosition, ypos, textFontSize);
              }
              else {
                tft.drawRightString("ON", unitPosition, ypos, textFontSize);
              }
              tft.setTextColor(COLOR_MENU_TEXT);
              if (UARTDebug) {
                tft.drawRightString("ON", unitPosition, ypos, textFontSize);
              }
              else {
                tft.drawRightString("OFF", unitPosition, ypos, textFontSize);
              }
              break;
            case setdefaultValuesGraphicPosition:
              loaddefaultValues();
              settings();
              break;
            case calibrateGraphicPosition:
              calibrateSensors();
              break;
          }
          break;
        case calibrateSensorsPage:
          switch (bar_pos - graphicTextOffset ) {
            case temperatureCalibrationGraphicPosition:
              while (digitalRead(ENC_SWITCH)) {
                updateData();
                if (EncMove) {
                  tft.setTextColor(COLOR_MENU);
                  tft.drawFloat(previousTemperature[babyNTC], 1, valuePosition, ypos, textFontSize);
                  tft.setTextColor(COLOR_MENU_TEXT);
                  diffTemperature[babyNTC] += EncMove * (0.1);
                  temperature[babyNTC] += EncMove * (0.1);
                  tft.drawFloat(temperature[babyNTC], 1, valuePosition, ypos, textFontSize);
                  previousTemperature[babyNTC] = temperature[babyNTC];
                  EncMove = false;
                  EEPROM.write(EEPROM_diffTemperature, int(diffTemperature[babyNTC] * 10));
                }
              }
              break;
            case humidityCalibrationGraphicPosition:
              while (digitalRead(ENC_SWITCH)) {
                updateData();
                if (EncMove) {
                  tft.setTextColor(COLOR_MENU);
                  tft.drawFloat(humidity, 0, valuePosition, ypos, textFontSize);
                  tft.setTextColor(COLOR_MENU_TEXT);
                  diffHumidity += EncMove;
                  humidity += EncMove;
                  tft.drawFloat(humidity, 0, valuePosition, ypos, textFontSize);
                  EncMove = false;
                  EEPROM.write(EEPROM_diffHumidity, diffHumidity );
                }
              }
              break;
            case restartCalibrationValuesTempGraphicPosition:
              diffTemperature[babyNTC] = false;
              diffHumidity = false;
              EEPROM.write(EEPROM_diffTemperature, diffTemperature[babyNTC]);
              EEPROM.write(EEPROM_diffHumidity, diffHumidity);
              calibrateSensors();
              break;
          }
          break;
      }
      selected = false;
      tft.fillRect(0, (tft.height() - height_heading) * (bar_pos - 1) / rectangles + height_heading, width_select, (tft.height() - height_heading) / rectangles, WHITE);
      while (!digitalRead(ENC_SWITCH)) {
        updateData();
        checkEncoderPress();
        if (page != mainMenuPage && page != askSuccessPage) {
          back_mode();
        }
      }
      delay(debounceTime);
    }
  }
}

void checkEncoderPress() {
  updateData();
  if (page == mainMenuPage) {
    long timePressed = millis();
    while (!digitalRead(ENC_SWITCH)) {
      updateData();
      if (millis() - timePressed > timePressToSettings) {
        settings();
      }
    }
  }
}

int getYpos(byte row) {
  return ((tft.height() - height_heading) / (2 * rectangles) + (row - 1) * (tft.height() - height_heading) / (rectangles) + letter_height);
}

void checkSetMessage() {
  int compareTime;
  if (blinkSetMessageState) {
    compareTime = blinkTimeON;
  }
  else {
    compareTime = blinkTimeOFF;
  }
  if (millis() - lastBlinkSetMessage > compareTime) {
    lastBlinkSetMessage = millis();
    blinkSetMessageState = !blinkSetMessageState;
    if (blinkSetMessageState) {
      tft.setTextColor(COLOR_WARNING_TEXT);

    }
    else {
      tft.setTextColor(COLOR_MENU);
    }
    if (page == mainMenuPage) {
      switch (language) {
        case english:
          helpMessage = "Set gestation weeks";
          break;
        case spanish:
          helpMessage = "Introduce S.D.G.";
          break;
        case french:
          helpMessage = "Regler semaines de gestation";
          break;
        case portuguese:
          helpMessage = "Entre semanas gestacao";
          break;
      }
    }
    else if (page == advancedModePage) {
      switch (language) {
        case english:
          helpMessage = "Set desired temperature";
          break;
        case spanish:
          helpMessage = "Introduce temperatura";
          break;
        case french:
          helpMessage = "Regler temperature desiree";
          break;
        case portuguese:
          helpMessage = "Insira a temperatura";
          break;
      }
    }
    tft.drawCentreString(helpMessage, width_select + (tft.width() - width_select) / 2, getYpos(goToProcessRow), textFontSize);

  }
}

void setPresetRoomValues() {
  if (gestationWeeks <= 29) {
    desiredSkinTemp = 36;
  }
  else {
    desiredSkinTemp = 36.5;
  }
  if (gestationWeeks <= 28) {
    desiredRoomHum = 80;
  }
  else if (gestationWeeks <= 32) {
    desiredRoomHum = 75;
  }
  else if (gestationWeeks <= 34) {
    desiredRoomHum = 55;
  }
  else if (gestationWeeks <= 35) {
    desiredRoomHum = 50;
  }
  else {
    desiredRoomHum = 30;
  }
}

void back_mode() {
  delay(debounceTime);
  last_encPulsed = millis();
  byte back_bar = false;
  while (!digitalRead(ENC_SWITCH)) {
    updateData();
    if (millis() - last_encPulsed > time_back_wait) {
      back_bar++;
      tft.drawLine(width_back - back_bar, 0, width_back - back_bar, height_heading, COLOR_MENU);
    }
    if (back_bar == width_back) {
      if (page == actuatorsProgressPage) {
        askSuccess();
      }
      else {
        mainMenu();
      }
    }
    delay((time_back_draw + time_back_wait) / width_back);
  }
  if (millis() - last_encPulsed > time_back_wait) {
    drawBack();
  }
  delay(50);
}
