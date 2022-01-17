
bool userInterfaceHandler() {
  backlightHandler();
  if (EncMove) {
    if (!selected) {
      if (EncMove < 0) {
        EncMove++;
        if (page == advancedModePage) {
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
        if (bar_pos > 1) {
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
      if (checkEncoderPress()) {
        return true;
      }
      if (page != advancedModePage) {
        if (back_mode()) {
          return true;
        }
      }
    }
    delay(debounceTime);
    switch (page) {
      case advancedModePage:
        switch (bar_pos - graphicTextOffset ) {
          case controlModeGraphicPosition:
            controlMode = !controlMode;
            EEPROM.write(EEPROM_controlMode, controlMode);
            EEPROM.commit();
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
                EncMove = false;
              }
            }
            EEPROM.write(EEPROM_language, language);
            EEPROM.commit();
            settings();
            break;
          case controlAlgorithmGraphicPosition:
            controlAlgorithm = !controlAlgorithm;
            EEPROM.write(EEPROM_controlAlgorithm, controlAlgorithm);
            EEPROM.commit();
            setTextColor(COLOR_MENU);
            if (controlAlgorithm) {
              drawRightString("BASIC", unitPosition, ypos, textFontSize);
            }
            else {
              drawRightString("PID", unitPosition, ypos, textFontSize);
            }
            setTextColor(COLOR_MENU_TEXT);
            if ((controlAlgorithm)) {
              drawRightString("PID", unitPosition, ypos, textFontSize);
            }
            else {
              drawRightString("BASIC", unitPosition, ypos, textFontSize);
            }
            break;
          case serialNumberGraphicPosition:
            while (GPIORead(ENC_SWITCH) ) {
              updateData();
              if (EncMove) {
                setTextColor(COLOR_MENU);
                drawRightNumber(serialNumber, 280, ypos);
                serialNumber -= EncMove;
                EEPROM.write(EEPROM_SerialNumber, serialNumber);
                setTextColor(COLOR_MENU_TEXT);
                drawRightNumber(serialNumber, 280, ypos);
              }
              EncMove = false;
            }
            EEPROM.commit();
            break;
          case WifiENGraphicPosition:
            WIFI_EN = !WIFI_EN;
            if (WIFI_EN) {
              wifiInit();
            }
            else {
              wifiDisable();
            }
            EEPROM.write(EEPROM_WIFI_EN, WIFI_EN);
            EEPROM.commit();
            setTextColor(COLOR_MENU);
            if (WIFI_EN) {
              drawRightString("OFF", unitPosition, ypos, textFontSize);
            }
            else {
              drawRightString("ON", unitPosition, ypos, textFontSize);
            }
            setTextColor(COLOR_MENU_TEXT);
            if (WIFI_EN) {
              drawRightString("ON", unitPosition, ypos, textFontSize);
            }
            else {
              drawRightString("OFF", unitPosition, ypos, textFontSize);
            }
            break;
          case setdefaultValuesGraphicPosition:
            loaddefaultValues();
            if (WIFI_EN) {
              wifiInit();
            }
            else {
              wifiDisable();
            }
            settings();
            break;
          case calibrateGraphicPosition:
            calibrateSensors();
            break;
        }
        break;
      case calibrateSensorsPage:
        switch (bar_pos - graphicTextOffset ) {
          case twoPointCalibrationGraphicPosition:
            firstPointCalibration();
            break;
          case restartCalibrationGraphicPosition:
            for (int i = 0; i < numTempSensors; i++) {
              RawTemperatureLow [i] = false;
              RawTemperatureRange [i] = false;
            }
            ReferenceTemperatureRange = false;
            ReferenceTemperatureLow = false;
            diffHumidity = false;
            /*
              EEPROM.writeFloat(EEPROM_diffAirTemperature, diffTemperature[airNTC]);
              EEPROM.writeFloat(EEPROM_diffSkinTemperature, diffTemperature[babyNTC]);
            */
            EEPROM.writeFloat(EEPROM_RawSkinTemperatureLowCorrection, RawTemperatureLow[babyNTC]);
            EEPROM.writeFloat(EEPROM_RawSkinTemperatureRangeCorrection, RawTemperatureRange[babyNTC]);
            EEPROM.writeFloat(EEPROM_RawAirTemperatureLowCorrection, RawTemperatureLow[airNTC]);
            EEPROM.writeFloat(EEPROM_RawAirTemperatureRangeCorrection, RawTemperatureRange[airNTC]);
            EEPROM.writeFloat(EEPROM_RawDigitalTemperatureLowCorrection, RawTemperatureLow[digitalTempSensor]);
            EEPROM.writeFloat(EEPROM_RawDigitalTemperatureRangeCorrection, RawTemperatureRange[digitalTempSensor]);
            EEPROM.writeFloat(EEPROM_ReferenceTemperatureRange, ReferenceTemperatureRange);
            EEPROM.writeFloat(EEPROM_ReferenceTemperatureLow, ReferenceTemperatureLow);
            EEPROM.write(EEPROM_diffHumidity, diffHumidity);
            EEPROM.commit();
            calibrateSensors();
            break;
        }
        break;
      case firstPointCalibrationPage:
        switch (bar_pos - graphicTextOffset ) {
          case temperatureCalibrationGraphicPosition:
            for (int i = 0; i < numTempSensors; i++) {
              errorTemperature[i] = false;
            }
            diffTemperature = temperature[babyNTC];
            while (GPIORead(ENC_SWITCH)) {
              updateData();
              if (EncMove) {
                setTextColor(COLOR_MENU);
                drawFloat(diffTemperature, 1, valuePosition, ypos, textFontSize);
                setTextColor(COLOR_MENU_TEXT);
                diffTemperature += EncMove * (0.1);
                drawFloat(diffTemperature, 1, valuePosition, ypos, textFontSize);
                EncMove = false;
              }
            }
            break;
          case setCalibrationGraphicPosition:
            ReferenceTemperatureLow = diffTemperature;
            for (int i = 0; i < numTempSensors; i++) {
              RawTemperatureLow[i] = temperature[i];
            }
            secondPointCalibration();
            break;
        }
        break;
      case secondPointCalibrationPage:
        switch (bar_pos - graphicTextOffset ) {
          case temperatureCalibrationGraphicPosition:
            diffTemperature = temperature[babyNTC];
            while (GPIORead(ENC_SWITCH)) {
              updateData();
              if (EncMove) {
                setTextColor(COLOR_MENU);
                drawFloat(diffTemperature, 1, valuePosition, ypos, textFontSize);
                setTextColor(COLOR_MENU_TEXT);
                diffTemperature += EncMove * (0.1);
                drawFloat(diffTemperature, 1, valuePosition, ypos, textFontSize);
                EncMove = false;
                Serial.println("difTemp: " + String(diffTemperature));
              }
            }
            break;
          case setCalibrationGraphicPosition:
            ReferenceTemperatureRange = diffTemperature - ReferenceTemperatureLow;
            if (temperatureCalibrationPoint != diffTemperature) {
              for (int i = 0; i < numTempSensors; i++) {
                RawTemperatureRange[i] = (temperature[i] - RawTemperatureLow[i]);
                Serial.println("calibration factors: " + String(RawTemperatureLow [i]) + "," + String (RawTemperatureRange [i]) + "," + String (ReferenceTemperatureRange) + "," + String (ReferenceTemperatureLow));
              }
              EEPROM.writeFloat(EEPROM_RawSkinTemperatureLowCorrection, RawTemperatureLow[babyNTC]);
              EEPROM.writeFloat(EEPROM_RawSkinTemperatureRangeCorrection, RawTemperatureRange[babyNTC]);
              EEPROM.writeFloat(EEPROM_RawAirTemperatureLowCorrection, RawTemperatureLow[airNTC]);
              EEPROM.writeFloat(EEPROM_RawAirTemperatureRangeCorrection, RawTemperatureRange[airNTC]);
              EEPROM.writeFloat(EEPROM_RawDigitalTemperatureLowCorrection, RawTemperatureLow[digitalTempSensor]);
              EEPROM.writeFloat(EEPROM_RawDigitalTemperatureRangeCorrection, RawTemperatureRange[digitalTempSensor]);
              EEPROM.writeFloat(EEPROM_ReferenceTemperatureRange, ReferenceTemperatureRange);
              EEPROM.writeFloat(EEPROM_ReferenceTemperatureLow, ReferenceTemperatureLow);
              EEPROM.commit();
            }
            else {
              logln("[CALIBRATION] -> ERROR -> DIVIDE BY ZERO");
            }
            settings();
            break;
        }
        break;
    }
    selected = false;
    tft.fillRect(0, (tft.height() - height_heading) * (bar_pos - 1) / rectangles + height_heading, width_select, (tft.height() - height_heading) / rectangles, WHITE);
    while (!GPIORead(ENC_SWITCH)) {
      updateData();
      checkEncoderPress();
      if (page != advancedModePage) {
        back_mode();
      }
    }
    delay(debounceTime);
  }
}

bool checkEncoderPress() {
  updateData();
  if (page == advancedModePage) {
    long timePressed = millis();
    while (!GPIORead(ENC_SWITCH)) {
      updateData();
      if (millis() - timePressed > timePressToSettings) {
        settings();
        return (true);
      }
    }
  }
  return false;
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
    if (page == advancedModePage) {
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

bool back_mode() {
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
      advancedMode();
      return true;
    }
    delay((time_back_draw + time_back_wait) / width_back);
  }
  if (millis() - last_encPulsed > time_back_wait) {
    drawBack();
  }
  delay(10);
  return (false);
}
