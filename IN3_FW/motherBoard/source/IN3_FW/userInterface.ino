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

void userInterfaceHandler(int UI_page, int UI_EncMove) {
  if (UI_EncMove) {
    if (!selected) {
      if (UI_EncMove < 0) {
        UI_EncMove++;
        if (UI_page == mainMenuPage) {
          enableSetProcess = enableSet;
        }
        else {
          enableSetProcess = true;
        }
        if (bar_pos < menu_rows - !enableSetProcess) {
          eraseBar(menu_rows, bar_pos);
          bar_pos++;
          updateBar(menu_rows, bar_pos);
        }
      }
      else {
        UI_EncMove --;
        if (bar_pos > 1) {
          eraseBar(menu_rows, bar_pos);
          bar_pos--;
          updateBar(menu_rows, bar_pos);
        }
      }
      ypos = graphicHeight(bar_pos - 1);
    }
  }
  if (!digitalRead(ENC_SWITCH)) {
    selected = ! selected;
    if (menu_rows) {
      if (selected) {
        tft.fillRect(0, (tft.height() - height_heading) * (bar_pos - 1) / menu_rows + height_heading, width_select, (tft.height() - height_heading) / menu_rows, COLOR_CHOSEN);
      }
      else {
        tft.fillRect(0, (tft.height() - height_heading) * (bar_pos - 1) / menu_rows + height_heading, width_select, (tft.height() - height_heading) / menu_rows, WHITE);
      }
      for (int i = 2; i <= menu_rows; i++) {
        tft.fillRect(0, (tft.height() - height_heading) * (i - 1) / menu_rows + height_heading - 1, tft.height(), width_indentation, WHITE); //mejorable
      }
    }
    if (!encoderContinuousPress(UI_page)) {
      switch (UI_page) {
        case mainMenuPage:
          switch (bar_pos - graphicTextOffset ) {
            case controlModeGraphicPosition:
              controlMode = !controlMode;
              EEPROM.write(EEPROM_controlMode, controlMode);
              EEPROM.commit();
              UI_mainMenu();
              break;
            case temperatureGraphicPosition:
              while (digitalRead(ENC_SWITCH)) {
                updateData();
                if (UI_EncMove) {
                  if (UI_EncMove > 0) {
                    if (desiredControlTemperature > minDesiredTemp[controlMode]) {
                      updateUIData = true;
                    }
                  }
                  else {
                    if (desiredControlTemperature < maxDesiredTemp[controlMode]) {
                      updateUIData = true;
                    }
                  }
                  if (updateUIData) {
                    setTextColor(COLOR_MENU);
                    if (!controlTemperature) {
                      controlTemperature = true;
                      drawRightString(initialSensorsValue, initialSensorPosition, temperatureY, textFontSize);
                    }
                    drawFloat(desiredControlTemperature, 1, temperatureX - 65, temperatureY, textFontSize);
                    desiredControlTemperature -= float(UI_EncMove) * stepTemperatureIncrement;
                    setTextColor(COLOR_MENU_TEXT);
                    drawFloat(desiredControlTemperature, 1, temperatureX - 65, temperatureY, textFontSize);
                    enableSet = true;
                  }
                  UI_EncMove = false;
                  updateUIData = false;
                }
              }
              EEPROM.write(EEPROM_desiredControlMode, desiredControlTemperature);
              EEPROM.commit();
              drawStartMessage(enableSet, menu_rows, helpMessage);
              break;
            case humidityGraphicPosition:
              while (digitalRead(ENC_SWITCH)) {
                updateData();
                if (UI_EncMove ) {
                  if (UI_EncMove > 0) {
                    if (desiredControlHumidity > minHum) {
                      updateUIData = true;
                    }
                  }
                  else {
                    if (desiredControlHumidity < maxHum) {
                      updateUIData = true;
                    }
                  }
                  if (updateUIData) {
                    setTextColor(COLOR_MENU);
                    if (!controlHumidity) {
                      controlHumidity = true;
                      drawRightString(initialSensorsValue, initialSensorPosition, humidityY, textFontSize);
                    }
                    drawCentreNumber(desiredControlHumidity, humidityX - 65, humidityY);
                    desiredControlHumidity -= (UI_EncMove) * stepHumidityIncrement;
                    setTextColor(COLOR_MENU_TEXT);
                    drawCentreNumber(desiredControlHumidity, humidityX - 65, humidityY);
                    enableSet = true;
                  }
                }
                UI_EncMove = false;
                updateUIData = false;
              }
              EEPROM.write(EEPROM_desiredControlHumidity, desiredControlHumidity);
              EEPROM.commit();
              drawStartMessage(enableSet, menu_rows, helpMessage);
              break;
            case LEDGraphicPosition:
              jaundiceEnable = !jaundiceEnable;
              setTextColor(COLOR_MENU);
              if (jaundiceEnable) {
                drawRightString(convertStringToChar(cstring, "OFF"), unitPosition, ypos, textFontSize);
              }
              else {
                drawRightString(convertStringToChar(cstring, "ON"), unitPosition, ypos, textFontSize);
              }
              setTextColor(COLOR_MENU_TEXT);
              if (jaundiceEnable) {
                drawRightString(convertStringToChar(cstring, "ON"), unitPosition, ypos, textFontSize);
                GPRSSetPostVariables(jaundiceLEDON, "");
                setGPRSPostPeriod(jaundiceGPRSPostPeriod);
              }
              else {
                drawRightString(convertStringToChar(cstring, "OFF"), unitPosition, ypos, textFontSize);
                GPRSSetPostVariables(jaundiceLEDOFF, "");
                GPRSSetPostVariables(jaundiceLEDOFF, "");
                setGPRSPostPeriod(standByGPRSPostPeriod);
              }
              digitalWrite(PHOTOTHERAPY, jaundiceEnable);
              break;
            case settingsGraphicPosition:
              UI_settings();
              break;
            case startGraphicPosition:
              UI_actuatorsProgress();
              break;
          }
          break;
        case settingsPage:
          switch (bar_pos - graphicTextOffset ) {
            case languageGraphicPosition:
              while (digitalRead(ENC_SWITCH)) {
                updateData();
                if (UI_EncMove) {
                  setTextColor(COLOR_MENU);
                  switch (language) {
                    case spanish:
                      textToWrite = convertStringToChar(cstring, "SPA");
                      break;
                    case english:
                      textToWrite = convertStringToChar(cstring, "ENG");
                      break;
                    case french:
                      textToWrite = convertStringToChar(cstring, "FRA");
                      break;
                    case portuguese:
                      textToWrite = convertStringToChar(cstring, "POR");
                      break;
                  }
                  drawRightString(textToWrite, unitPosition, ypos, textFontSize);
                  language -= UI_EncMove;
                  if (language < 0) {
                    language = numLanguages - 1;
                  }
                  if (language >= numLanguages) {
                    language = false;
                  }
                  setTextColor(COLOR_MENU_TEXT);
                  switch (language) {
                    case spanish:
                      textToWrite = convertStringToChar(cstring, "SPA");
                      break;
                    case english:
                      textToWrite = convertStringToChar(cstring, "ENG");
                      break;
                    case french:
                      textToWrite = convertStringToChar(cstring, "FRA");
                      break;
                    case portuguese:
                      textToWrite = convertStringToChar(cstring, "POR");
                      break;
                  }
                  drawRightString(textToWrite, unitPosition, ypos, textFontSize);
                  UI_EncMove = false;
                }
              }
              EEPROM.write(EEPROM_language, language);
              EEPROM.commit();
              UI_settings();
              break;
            case serialNumberGraphicPosition:
              while (digitalRead(ENC_SWITCH) ) {
                updateData();
                if (UI_EncMove) {
                  setTextColor(COLOR_MENU);
                  drawRightNumber(serialNumber, unitPosition, ypos);
                  serialNumber -= UI_EncMove;
                  EEPROM.write(EEPROM_SerialNumber, serialNumber);
                  setTextColor(COLOR_MENU_TEXT);
                  drawRightNumber(serialNumber, unitPosition, ypos);
                }
                UI_EncMove = false;
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
                drawRightString(convertStringToChar(cstring, "OFF"), unitPosition, ypos, textFontSize);
              }
              else {
                drawRightString(convertStringToChar(cstring, "ON"), unitPosition, ypos, textFontSize);
              }
              setTextColor(COLOR_MENU_TEXT);
              if (WIFI_EN) {
                drawRightString(convertStringToChar(cstring, "ON"), unitPosition, ypos, textFontSize);
              }
              else {
                drawRightString(convertStringToChar(cstring, "OFF"), unitPosition, ypos, textFontSize);
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
              UI_settings();
              break;
            case HWTestGraphicPosition:
              initHardware(true);
              UI_settings();
              break;
            case calibrateGraphicPosition:
              UI_calibration();
              break;
          }
          break;
        case calibrateSensorsPage:
          switch (bar_pos - graphicTextOffset ) {
            case twoPointCalibrationGraphicPosition:
              firstPointCalibration();
              break;
            case fineTuneCalibrationGraphicPosition:
              fineTuneCalibration();
              break;
            case autoCalibrationGraphicPosition:
              autoCalibration();
              break;
            case restartCalibrationGraphicPosition:
              loadDefaultCalibration();
              recapVariables();
              UI_calibration();
              break;
          }
          break;
        case fineTuneCalibrationPage:
          switch (bar_pos - graphicTextOffset ) {
            case temperatureCalibrationGraphicPosition:
              errorTemperature[skinSensor] = false;
              diffTemperature = temperature[skinSensor];
              while (digitalRead(ENC_SWITCH)) {
                updateData();
                if (UI_EncMove) {
                  setTextColor(COLOR_MENU);
                  drawFloat(diffTemperature, 1, valuePosition, ypos, textFontSize);
                  setTextColor(COLOR_MENU_TEXT);
                  diffTemperature += UI_EncMove * (0.1);
                  drawFloat(diffTemperature, 1, valuePosition, ypos, textFontSize);
                  UI_EncMove = false;
                }
              }
              break;
            case setCalibrationGraphicPosition:
              fineTuneSkinTemperature = diffTemperature - temperature[skinSensor];
              logln("[CALIBRATION] -> Fine tune value is " + String(fineTuneSkinTemperature));
              EEPROM.writeFloat(EEPROM_FineTuneSkinTemperature, fineTuneSkinTemperature);
              EEPROM.commit();
              UI_mainMenu();
              break;
          }
          break;
        case firstPointCalibrationPage:
          clearCalibrationValues();
          switch (bar_pos - graphicTextOffset ) {
            case temperatureCalibrationGraphicPosition:
              errorTemperature[skinSensor] = false;
              diffTemperature = temperature[skinSensor];
              while (digitalRead(ENC_SWITCH)) {
                updateData();
                if (UI_EncMove) {
                  setTextColor(COLOR_MENU);
                  drawFloat(diffTemperature, 1, valuePosition, ypos, textFontSize);
                  setTextColor(COLOR_MENU_TEXT);
                  diffTemperature += UI_EncMove * (0.1);
                  drawFloat(diffTemperature, 1, valuePosition, ypos, textFontSize);
                  UI_EncMove = false;
                }
              }
              break;
            case setCalibrationGraphicPosition:
              provisionalReferenceTemperatureLow = diffTemperature;
              provisionalRawTemperatureLow[skinSensor] = temperature[skinSensor];
              logln("[CALIBRATION] -> Low reference point is " + String(provisionalReferenceTemperatureLow) +  ", low raw skin point is " + String(provisionalRawTemperatureLow[skinSensor]));
              secondPointCalibration();
              break;
          }
          break;
        case secondPointCalibrationPage:
          switch (bar_pos - graphicTextOffset ) {
            case temperatureCalibrationGraphicPosition:
              diffTemperature = temperature[skinSensor];
              while (digitalRead(ENC_SWITCH)) {
                updateData();
                if (UI_EncMove) {
                  setTextColor(COLOR_MENU);
                  drawFloat(diffTemperature, 1, valuePosition, ypos, textFontSize);
                  setTextColor(COLOR_MENU_TEXT);
                  diffTemperature += UI_EncMove * (0.1);
                  drawFloat(diffTemperature, 1, valuePosition, ypos, textFontSize);
                  UI_EncMove = false;
                  logln("difTemp: " + String(diffTemperature));
                }
              }
              break;
            case setCalibrationGraphicPosition:
              ReferenceTemperatureLow = provisionalReferenceTemperatureLow;
              RawTemperatureLow[skinSensor] = provisionalRawTemperatureLow[skinSensor];
              ReferenceTemperatureRange = diffTemperature - ReferenceTemperatureLow;
              if (RawTemperatureRange[skinSensor]) {
                RawTemperatureRange[skinSensor] = (temperature[skinSensor] - RawTemperatureLow[skinSensor]);
                logln("calibration factors: " + String(RawTemperatureLow [skinSensor]) + "," + String (RawTemperatureRange [skinSensor]) + "," + String (ReferenceTemperatureRange) + "," + String (ReferenceTemperatureLow));
                saveCalibrationToEEPROM();
              }
              else {
                logln("[CALIBRATION] -> ERROR -> DIVIDE BY ZERO");
              }
              UI_settings();
              break;
          }
          break;
        case autoCalibrationPage:
          break;
      }
      selected = false;
      if (menu_rows) {
        tft.fillRect(0, (tft.height() - height_heading) * (bar_pos - 1) / menu_rows + height_heading, width_select, (tft.height() - height_heading) / menu_rows, WHITE);
      }
      encoderContinuousPress(UI_page);
      vTaskDelay(debounceTime);
    }
  }
}

bool encoderContinuousPress(int UI_page) {
  updateData();
  if (UI_page == mainMenuPage) {
    long timePressed = millis();
    while (!digitalRead(ENC_SWITCH)) {
      updateData();
      if (millis() - timePressed > timePressToSettings) {
        UI_settings();
        return (true);
      }
    }
  }
  else {
    return (back_mode());
  }
  return false;
}

int getYpos(int UI_menu_rows, byte row) {
  row++; //because it starts at zero
  if (UI_menu_rows) {
    return ((tft.height() - height_heading) / (2 * UI_menu_rows) + (row - 1) * (tft.height() - height_heading) / (menu_rows) + letter_height);
  }
  return false;
}

void checkSetMessage(int UI_page)
{
  char *UI_helpMessage;
  int compareTime;
  if (blinkSetMessageState)
  {
    compareTime = blinkTimeON;
  }
  else
  {
    compareTime = blinkTimeOFF;
  }
  if (millis() - lastBlinkSetMessage > compareTime)
  {
    lastBlinkSetMessage = millis();
    blinkSetMessageState = !blinkSetMessageState;
    if (blinkSetMessageState)
    {
      setTextColor(COLOR_WARNING_TEXT);
    }
    else
    {
      setTextColor(COLOR_MENU);
    }
    if (UI_page == mainMenuPage)
    {
      switch (language)
      {
        case english:
          UI_helpMessage = convertStringToChar(cstring, "Set desired parameters");
          break;
        case spanish:
          UI_helpMessage = convertStringToChar(cstring, "Introduce parametros");
          break;
        case french:
          UI_helpMessage = convertStringToChar(cstring, "Entrer parametres");
          break;
        case portuguese:
          UI_helpMessage = convertStringToChar(cstring, "Insira os parametros");
          break;
      }
    }
    drawCentreString(UI_helpMessage, width_select + (tft.width() - width_select) / 2, getYpos(menu_rows, startGraphicPosition), textFontSize);
  }
}


bool back_mode() {
  vTaskDelay(debounceTime);
  last_encPulsed = millis();
  byte back_bar = false;
  while (!digitalRead(ENC_SWITCH)) {
    updateData();
    if (millis() - last_encPulsed > time_back_wait) {
      back_bar++;
      tft.drawLine(width_back - back_bar, 0, width_back - back_bar, height_heading, COLOR_MENU);
    }
    if (back_bar == width_back) {
      UI_mainMenu();
      return true;
    }
    vTaskDelay((time_back_draw + time_back_wait) / width_back);
  }
  if (millis() - last_encPulsed > time_back_wait) {
    drawBack();
  }
  vTaskDelay(10);
  return (false);
}
