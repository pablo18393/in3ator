void barSelection() {
  bar_pos = 1;
  selected = 0;
  ypos = (tft.height() - height_heading) / (2 * rectangles) + letter_height;
  while (1) {
    updateData();
    if (EncMove) {
      if (!selected) {
        if (EncMove < 0) {
          EncMove++;
          if (!page) {
            enableSetProcess = enableSet;
          }
          else {
            enableSetProcess = 1;
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
    if (!digitalRead(pulse)) {
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
      while (!digitalRead(pulse)) {
        checkEncoderPress();
        if (page != menuPage) {
          back_mode();
        }
      }
      delay(debounceTime);
      switch (page) {
        case menuPage:
          switch (bar_pos - graphicTextOffset) {
            case temperatureGraphicPosition:
              while (digitalRead(pulse)) {
                updateData();
                if (EncMove && -EncMove + desiredRoomTemp >= minTemp && -EncMove + desiredRoomTemp <= maxTemp) {
                  tft.setTextColor(COLOR_MENU);
                  if (!controlTemperature) {
                    controlTemperature = 1;
                    tft.drawRightString(initialSensorsValue, initialSensorPosition, temperatureY, textFontSize);
                  }
                  tft.drawFloat(desiredRoomTemp, 1, temperatureX - 65, temperatureY, textFontSize);
                  desiredRoomTemp -= float(EncMove) / 10;
                  tft.setTextColor(COLOR_MENU_TEXT);
                  tft.drawFloat(desiredRoomTemp, 1, temperatureX - 65, temperatureY, textFontSize);
                  enableSet = 1;
                }
                EncMove = 0;
              }
              drawStartMessage();
              break;
            case humidityGraphicPosition:
              while (digitalRead(pulse)) {
                updateData();
                if (EncMove && -EncMove + desiredRoomHum >= minHum && -EncMove + desiredRoomHum <= maxHum) {
                  tft.setTextColor(COLOR_MENU);
                  if (!controlHumidity) {
                    controlHumidity = 1;
                    tft.drawRightString(initialSensorsValue, initialSensorPosition, humidityY, textFontSize);
                  }
                  drawCentreNumber(desiredRoomHum, humidityX - 65, humidityY);
                  desiredRoomHum -= (EncMove);
                  tft.setTextColor(COLOR_MENU_TEXT);
                  drawCentreNumber(desiredRoomHum, humidityX - 65, humidityY);
                  enableSet = 1;
                }
                EncMove = 0;
              }
              drawStartMessage();
              break;
            case LEDGraphicPosition:
              while (digitalRead(pulse) ) {
                updateData();
                if (EncMove && -EncMove + LEDIntensity >= 0 && -EncMove + LEDIntensity <= LEDMaxIntensity) {
                  tft.setTextColor(COLOR_MENU);
                  drawRightNumber(LEDIntensity, 280, ypos);
                  if (!LEDIntensity && EncMove) {
                    tft.drawRightString("OFF", unitPosition, ypos, textFontSize);
                    tft.setTextColor(COLOR_MENU_TEXT);
                    tft.drawRightString("%", unitPosition, ypos, textFontSize);
                  }
                  LEDIntensity -= 10 * EncMove;
                  analogWrite(ICT, LEDIntensity);
                  tft.setTextColor(COLOR_MENU_TEXT);
                  if (!LEDIntensity && EncMove) {
                    tft.setTextColor(COLOR_MENU);
                    tft.drawRightString("%", unitPosition, ypos, textFontSize);
                    tft.setTextColor(COLOR_MENU_TEXT);
                    tft.drawRightString("OFF", unitPosition, ypos, textFontSize);
                  }
                  else {
                    drawRightNumber(LEDIntensity, 280, ypos);
                  }
                }
                EncMove = 0;
              }
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
          switch (bar_pos - graphicTextOffset) {
            case autoLockGraphicPosition:
              while (digitalRead(pulse)) {
                updateData();
                if (EncMove) {
                  tft.setTextColor(COLOR_MENU);
                  if (auto_lock) {
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
                    }
                    tft.drawRightString(textToWrite, unitPosition, ypos, textFontSize);
                  }
                  auto_lock = !auto_lock;
                  EEPROM.write(EEPROM_autoLock, auto_lock);
                  EncMove = 0;
                }
              }
              break;
            case languageGraphicPosition:
              while (digitalRead(pulse)) {
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
                  }
                  tft.drawRightString(textToWrite, unitPosition, ypos, textFontSize);
                  language -= EncMove;
                  if (language < firstLanguage) {
                    language = numLanguages;
                  }
                  if (language > numLanguages) {
                    language = firstLanguage;
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
                  }
                  tft.drawRightString(textToWrite, unitPosition, ypos, textFontSize);
                  EEPROM.write(EEPROM_language, language);
                  EncMove = 0;
                }
              }
              settings();
              break;
            case heaterTempGraphicPosition:
              while (digitalRead(pulse) ) {
                updateData();
                if (EncMove && -EncMove + heaterTempLimit >= 0 && -EncMove + heaterTempLimit <= heaterMaxTemp) {
                  tft.setTextColor(COLOR_MENU);
                  drawRightNumber(heaterTempLimit, 280, ypos);
                  if (!heaterTempLimit && EncMove) {
                    tft.drawRightString("OFF", unitPosition, ypos, textFontSize);
                    tft.setTextColor(COLOR_MENU_TEXT);
                    tft.drawRightString("C", unitPosition, ypos, textFontSize);
                  }
                  heaterTempLimit -= 5 * EncMove;
                  EEPROM.write(EEPROM_heaterTempLimit, heaterTempLimit);
                  tft.setTextColor(COLOR_MENU_TEXT);
                  if (!heaterTempLimit && EncMove) {
                    tft.setTextColor(COLOR_MENU);
                    tft.drawRightString("C", unitPosition, ypos, textFontSize);
                    tft.setTextColor(COLOR_MENU_TEXT);
                    tft.drawRightString("OFF", unitPosition, ypos, textFontSize);
                  }
                  else {
                    drawRightNumber(heaterTempLimit, 280, ypos);
                  }
                }
                EncMove = 0;
              }
              break;
            case fanGraphicPosition:
              while (digitalRead(pulse)) {
                updateData();
                turnFansOn();
                if (EncMove && -EncMove + fanSpeed >= 0 && -EncMove + fanSpeed <= fanMaxSpeed) {
                  tft.setTextColor(COLOR_MENU);
                  drawRightNumber(fanSpeed, 280, ypos);
                  if (!fanSpeed && EncMove) {
                    tft.drawRightString("OFF", unitPosition, ypos, textFontSize);
                    tft.setTextColor(COLOR_MENU_TEXT);
                    tft.drawRightString("%", unitPosition, ypos, textFontSize);
                  }
                  fanSpeed -= 5 * EncMove;
                  EEPROM.write(EEPROM_fanSpeed, fanSpeed);
                  tft.setTextColor(COLOR_MENU_TEXT);
                  if (!fanSpeed && EncMove) {
                    tft.setTextColor(COLOR_MENU);
                    tft.drawRightString("%", unitPosition, ypos, textFontSize);
                    tft.setTextColor(COLOR_MENU_TEXT);
                    tft.drawRightString("OFF", unitPosition, ypos, textFontSize);
                  }
                  else {
                    drawRightNumber(fanSpeed, 280, ypos);
                  }
                }
                EncMove = 0;
              }
              turnFansOff();
              break;
            case setStandardValuesGraphicPosition:
              loadStandardValues();
              settings();
              break;
            case calibrateGraphicPosition:
              calibrateSensors();
              break;
          }
          break;
        case calibrateSensorsPage:
          switch (bar_pos - graphicTextOffset) {
            case temperatureCalibrationGraphicPosition:
              while (digitalRead(pulse)) {
                updateData();
                if (EncMove) {
                  tft.setTextColor(COLOR_MENU);
                  tft.drawFloat(previousTemperature[roomNTC], 1, valuePosition, ypos, textFontSize);
                  tft.setTextColor(COLOR_MENU_TEXT);
                  diffTemperature[roomNTC] += EncMove * (0.1);
                  temperature[roomNTC] += EncMove * (0.1);
                  tft.drawFloat(temperature[roomNTC], 1, valuePosition, ypos, textFontSize);
                  previousTemperature[roomNTC] = temperature[roomNTC];
                  EncMove = 0;
                  EEPROM.write(EEPROM_diffTemperature, int(diffTemperature[roomNTC] * 10));
                }
              }
              break;
            case humidityCalibrationGraphicPosition:
              while (digitalRead(pulse)) {
                updateData();
                if (EncMove) {
                  tft.setTextColor(COLOR_MENU);
                  tft.drawFloat(humidity, 0, valuePosition, ypos, textFontSize);
                  tft.setTextColor(COLOR_MENU_TEXT);
                  diffHumidity += EncMove;
                  humidity += EncMove;
                  tft.drawFloat(humidity, 0, valuePosition, ypos, textFontSize);
                  EncMove = 0;
                  EEPROM.write(EEPROM_diffHumidity, diffHumidity );
                }
              }
              break;
            case restartCalibrationValuesTempGraphicPosition:
              diffTemperature[roomNTC] = 0;
              diffHumidity = 0;
              EEPROM.write(EEPROM_diffTemperature, diffTemperature[roomNTC]);
              EEPROM.write(EEPROM_diffHumidity, diffHumidity);
              readHumSensor();
              updateTemp(numNTC);
              calibrateSensors();
              break;
          }
          break;
      }
      selected = 0;
      tft.fillRect(0, (tft.height() - height_heading) * (bar_pos - 1) / rectangles + height_heading, width_select, (tft.height() - height_heading) / rectangles, WHITE);
      while (!digitalRead(pulse)) {
        checkEncoderPress();
        if (page != menuPage) {
          back_mode();
        }
      }
      delay(debounceTime);
    }
  }
}

void checkEncoderPress() {
  updateData();
  if (page == menuPage) {
    long timePressed = millis();
    while (!digitalRead(pulse)) {
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
    }
    tft.drawCentreString(helpMessage, width_select + (tft.width() - width_select) / 2, getYpos(goToProcessRow), textFontSize);
  }
}

void back_mode() {
  delay(debounceTime);
  last_pulsed = millis();
  byte back_bar = 0;
  while (!digitalRead(pulse)) {
    updateData();
    if (millis() - last_pulsed > time_back_wait) {
      back_bar++;
      tft.drawLine(width_back - back_bar, 0, width_back - back_bar, height_heading, COLOR_MENU);
    }
    if (back_bar == width_back) {
      if (page == actuatorsProgressPage) {
        askSuccess();
      }
      else {
        menu();
      }
    }
    delay((time_back_draw + time_back_wait) / width_back);
  }
  if (millis() - last_pulsed > time_back_wait) {
    drawBack();
  }
  delay(50);
}
