
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
    if (!GPIORead(ENC_SWITCH)) {
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
      while (!GPIORead(ENC_SWITCH)) {
        updateData();
        checkEncoderPress();
        if (page != mainMenuPage && page != askSuccessPage && page != advancedModePage) {
          back_mode();
        }
      }
      delay(debounceTime);
      switch (page) {
        case mainMenuPage:
          switch (bar_pos - graphicTextOffset ) {
            case gestationGraphicPosition:
              while (GPIORead(ENC_SWITCH)) {
                updateData();
                if (EncMove && -EncMove + gestationWeeks >= minGestation && -EncMove + gestationWeeks <= maxGestation) {
                  setTextColor(COLOR_MENU);
                  controlTemperature = true;
                  controlHumidity = true;
                  drawRightString(initialSensorsValue, gestationWeeksXPos, ypos, textFontSize);
                  drawRightNumber(gestationWeeks, gestationWeeksXPos, ypos);
                  gestationWeeks -= EncMove;
                  setPresetRoomValues();
                  setTextColor(COLOR_MENU_TEXT);
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
                drawRightString("OFF", unitPosition, ypos, textFontSize);
              }
              else {
                drawRightString("ON", unitPosition, ypos, textFontSize);
              }
              tft.setTextColor(COLOR_MENU_TEXT);
              if (jaundiceEnable) {
                drawRightString("ON", unitPosition, ypos, textFontSize);
                GPRSSetPostVariables(jaundiceLEDON, "");
                setGPRSPostPeriod(jaundiceGPRSPostPeriod);
              }
              else {
                drawRightString("OFF", unitPosition, ypos, textFontSize);
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
            case controlModeGraphicPosition:
              controlMode = !controlMode;
              EEPROM.write(EEPROM_controlMode, controlMode);
              advancedMode();
              break;
            case temperatureGraphicPosition:
              while (GPIORead(ENC_SWITCH)) {
                updateData();
                if (EncMove && -EncMove + desiredSkinTemp >= minTemp && -EncMove + desiredSkinTemp <= maxTemp) {
                  setTextColor(COLOR_MENU);
                  if (!controlTemperature) {
                    controlTemperature = true;
                    drawRightString(initialSensorsValue, initialSensorPosition, temperatureY, textFontSize);
                  }
                  drawFloat(desiredSkinTemp, 1, temperatureX - 65, temperatureY, textFontSize);
                  desiredSkinTemp -= float(EncMove) / 10;
                  setTextColor(COLOR_MENU_TEXT);
                  drawFloat(desiredSkinTemp, 1, temperatureX - 65, temperatureY, textFontSize);
                  enableSet = true;
                }
                EncMove = false;
              }
              drawStartMessage();
              break;
            case humidityGraphicPosition:
              while (GPIORead(ENC_SWITCH)) {
                updateData();
                if (EncMove && -EncMove + desiredRoomHum >= minHum && -EncMove + desiredRoomHum <= maxHum) {
                  setTextColor(COLOR_MENU);
                  if (!controlHumidity) {
                    controlHumidity = true;
                    drawRightString(initialSensorsValue, initialSensorPosition, humidityY, textFontSize);
                  }
                  drawCentreNumber(desiredRoomHum, humidityX - 65, humidityY);
                  desiredRoomHum -= (EncMove);
                  setTextColor(COLOR_MENU_TEXT);
                  drawCentreNumber(desiredRoomHum, humidityX - 65, humidityY);
                  enableSet = true;
                }
                EncMove = false;
              }
              drawStartMessage();
              break;
            case LEDGraphicPosition:
              jaundiceEnable = !jaundiceEnable;
              setTextColor(COLOR_MENU);
              if (jaundiceEnable) {
                drawRightString("OFF", unitPosition, ypos, textFontSize);
              }
              else {
                drawRightString("ON", unitPosition, ypos, textFontSize);
              }
              setTextColor(COLOR_MENU_TEXT);
              if (jaundiceEnable) {
                drawRightString("ON", unitPosition, ypos, textFontSize);
                GPRSSetPostVariables(jaundiceLEDON, "");
                setGPRSPostPeriod(jaundiceGPRSPostPeriod);
              }
              else {
                drawRightString("OFF", unitPosition, ypos, textFontSize);
                GPRSSetPostVariables(jaundiceLEDOFF, "");
                GPRSSetPostVariables(jaundiceLEDOFF, "");
                setGPRSPostPeriod(standByGPRSPostPeriod);
              }
              GPIOWrite(PHOTOTHERAPY, jaundiceEnable);
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
          advancedMode();
          break;
        case settingsPage:
          switch (bar_pos - graphicTextOffset ) {
            case autoLockGraphicPosition:
              setTextColor(COLOR_MENU);
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
                drawRightString(textToWrite, unitPosition, ypos, textFontSize);
                setTextColor(COLOR_MENU_TEXT);
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
                drawRightString(textToWrite, unitPosition, ypos, textFontSize);
              }
              else {
                setTextColor(COLOR_MENU);
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
                drawRightString(textToWrite, unitPosition, ypos, textFontSize);
                setTextColor(COLOR_MENU_TEXT);
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
                drawRightString(textToWrite, unitPosition, ypos, textFontSize);
              }
              autoLock = !autoLock;
              EEPROM.write(EEPROM_autoLock, autoLock);
              EEPROM.commit();
              break;
            case languageGraphicPosition:
              while (GPIORead(ENC_SWITCH)) {
                updateData();
                if (EncMove) {
                  setTextColor(COLOR_MENU);
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
                  drawRightString(textToWrite, unitPosition, ypos, textFontSize);
                  language -= EncMove;
                  if (language < 0) {
                    language = numLanguages - 1;
                  }
                  if (language >= numLanguages) {
                    language = false;
                  }
                  setTextColor(COLOR_MENU_TEXT);
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
                  drawRightString(textToWrite, unitPosition, ypos, textFontSize);
                  EEPROM.write(EEPROM_language, language);
                  EEPROM.commit();
                  EncMove = false;
                }
              }
              settings();
              break;
            case controlAlgorithmGraphicPosition:
              controlAlgorithm = !controlAlgorithm;
              EEPROM.write(EEPROM_controlAlgorithm, controlAlgorithm);
              EEPROM.commit();
              setTextColor(COLOR_MENU);
              if (controlAlgorithm) {
                drawRightString("BASIC", unitPosition + 10, ypos, textFontSize);
              }
              else {
                drawRightString("PID", unitPosition, ypos, textFontSize);
              }
              setTextColor(COLOR_MENU_TEXT);
              if ((controlAlgorithm)) {
                drawRightString("PID", unitPosition, ypos, textFontSize);
              }
              else {
                drawRightString("BASIC", unitPosition + 10, ypos, textFontSize);
              }
              break;
            case heaterPowerGraphicPosition:
              while (GPIORead(ENC_SWITCH) ) {
                updateData();
                if (EncMove && -EncMove + HeaterPower >= heaterPowerMin && -EncMove + HeaterPower <= heaterPowerMax) {
                  setTextColor(COLOR_MENU);
                  drawRightNumber(HeaterPower, 280, ypos);
                  HeaterPower -= 10 * EncMove;
                  EEPROM.write(EEPROM_HeaterPower, HeaterPower);
                  EEPROM.commit();
                  setTextColor(COLOR_MENU_TEXT);
                  drawRightNumber(HeaterPower, 280, ypos);
                }
                EncMove = false;
              }
              break;
            case DebugENGraphicPosition:
              UARTDebug = !UARTDebug;
              EEPROM.write(EEPROM_UARTDebug, UARTDebug);
              EEPROM.commit();
              setTextColor(COLOR_MENU);
              if (UARTDebug) {
                drawRightString("OFF", unitPosition, ypos, textFontSize);
              }
              else {
                drawRightString("ON", unitPosition, ypos, textFontSize);
              }
              setTextColor(COLOR_MENU_TEXT);
              if (UARTDebug) {
                drawRightString("ON", unitPosition, ypos, textFontSize);
              }
              else {
                drawRightString("OFF", unitPosition, ypos, textFontSize);
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
            case airTemperatureCalibrationGraphicPosition:
              while (GPIORead(ENC_SWITCH)) {
                updateData();
                if (EncMove) {
                  setTextColor(COLOR_MENU);
                  drawFloat(previousTemperature[airNTC], 1, valuePosition, ypos, textFontSize);
                  setTextColor(COLOR_MENU_TEXT);
                  diffTemperature[airNTC] += EncMove * (0.1);
                  temperature[airNTC] += EncMove * (0.1);
                  drawFloat(temperature[airNTC], 1, valuePosition, ypos, textFontSize);
                  previousTemperature[airNTC] = temperature[airNTC];
                  EncMove = false;
                  EEPROM.writeFloat(EEPROM_diffAirTemperature, diffTemperature[airNTC]);
                }
              }
              EEPROM.commit();
              break;
            case skinTemperatureCalibrationGraphicPosition:
              while (GPIORead(ENC_SWITCH)) {
                updateData();
                if (EncMove) {
                  setTextColor(COLOR_MENU);
                  drawFloat(previousTemperature[babyNTC], 1, valuePosition, ypos, textFontSize);
                  setTextColor(COLOR_MENU_TEXT);
                  diffTemperature[babyNTC] += EncMove * (0.1);
                  temperature[babyNTC] += EncMove * (0.1);
                  drawFloat(temperature[babyNTC], 1, valuePosition, ypos, textFontSize);
                  previousTemperature[babyNTC] = temperature[babyNTC];
                  EncMove = false;
                  EEPROM.writeFloat(EEPROM_diffSkinTemperature, diffTemperature[babyNTC]);
                }
              }
              EEPROM.commit();
              break;
            case humidityCalibrationGraphicPosition:
              while (GPIORead(ENC_SWITCH)) {
                updateData();
                if (EncMove) {
                  setTextColor(COLOR_MENU);
                  drawFloat(humidity, 0, valuePosition, ypos, textFontSize);
                  setTextColor(COLOR_MENU_TEXT);
                  diffHumidity += EncMove;
                  humidity += EncMove;
                  drawFloat(humidity, 0, valuePosition, ypos, textFontSize);
                  EncMove = false;
                  EEPROM.write(EEPROM_diffHumidity, diffHumidity );
                }
              }
              EEPROM.commit();
              break;
            case restartCalibrationGraphicPosition:
              diffTemperature[babyNTC] = false;
              diffTemperature[airNTC] = false;
              diffHumidity = false;
              EEPROM.writeFloat(EEPROM_diffAirTemperature, diffTemperature[airNTC]);
              EEPROM.writeFloat(EEPROM_diffSkinTemperature, diffTemperature[babyNTC]);
              EEPROM.write(EEPROM_diffHumidity, diffHumidity);
              EEPROM.commit();
              calibrateSensors();
              break;
            case autoCalibrationGraphicPosition:
              diffTemperature[babyNTC] = temperature[digitalTempSensor] - temperature[babyNTC];
              diffTemperature[airNTC] = temperature[digitalTempSensor] - temperature[airNTC];
              EEPROM.writeFloat(EEPROM_diffAirTemperature, diffTemperature[airNTC]);
              EEPROM.writeFloat(EEPROM_diffSkinTemperature, diffTemperature[babyNTC]);
              EEPROM.commit();
              calibrateSensors();
              break;
          }
          break;
      }
      selected = false;
      tft.fillRect(0, (tft.height() - height_heading) * (bar_pos - 1) / rectangles + height_heading, width_select, (tft.height() - height_heading) / rectangles, WHITE);
      while (!GPIORead(ENC_SWITCH)) {
        updateData();
        checkEncoderPress();
        if (page != mainMenuPage && page != askSuccessPage && page != advancedModePage) {
          back_mode();
        }
      }
      delay(debounceTime);
    }
  }
}

void checkEncoderPress() {
  updateData();
  if (page == mainMenuPage || page == advancedModePage) {
    long timePressed = millis();
    while (!GPIORead(ENC_SWITCH)) {
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
      setTextColor(COLOR_WARNING_TEXT);

    }
    else {
      setTextColor(COLOR_MENU);
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
          helpMessage = "Set desired parameters";
          break;
        case spanish:
          helpMessage = "Introduce parametros";
          break;
        case french:
          helpMessage = "Entrer parametres";
          break;
        case portuguese:
          helpMessage = "Insira os parametros";
          break;
      }
    }
    drawCentreString(helpMessage, width_select + (tft.width() - width_select) / 2, getYpos(goToProcessRow), textFontSize);

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
  while (!GPIORead(ENC_SWITCH)) {
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
        advancedMode();
      }
    }
    delay((time_back_draw + time_back_wait) / width_back);
  }
  if (millis() - last_encPulsed > time_back_wait) {
    drawBack();
  }
  delay(50);
}
