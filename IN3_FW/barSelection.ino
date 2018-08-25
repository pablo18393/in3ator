void barSelection() {
  bar_pos = 1;
  selected = 0;
  ypos = (tft.height() - height_heading) / (2 * rectangles) + letter_height;
  while (1) {
    updateData();
    if (move) {
      if (!selected) {
        if (move < 0) {
          move++;
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
          move --;
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
        updateData();
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
                if (move && -move + desiredIn3Temp >= minTemp && -move + desiredIn3Temp <= maxTemp) {
                  tft.setTextColor(COLOR_MENU);
                  if (!controlTemperature) {
                    controlTemperature = 1;
                    tft.drawRightString(initialSensorsValue, initialSensorPosition, temperatureY, textFontSize);
                  }
                  tft.drawFloat(desiredIn3Temp, 1, temperatureX - 65, temperatureY, textFontSize);
                  desiredIn3Temp -= float(move) / 10;
                  tft.setTextColor(COLOR_MENU_TEXT);
                  tft.drawFloat(desiredIn3Temp, 1, temperatureX - 65, temperatureY, textFontSize);
                  enableSet = 1;
                }
                move = 0;
              }
              drawStartMessage();
              break;
            case humidityGraphicPosition:
              while (digitalRead(pulse)) {
                updateData();
                if (move && -move + desiredIn3Hum >= minHum && -move + desiredIn3Hum <= maxHum) {
                  tft.setTextColor(COLOR_MENU);
                  if (!controlHumidity) {
                    controlHumidity = 1;
                    tft.drawRightString(initialSensorsValue, initialSensorPosition, humidityY, textFontSize);
                  }
                  drawCentreNumber(desiredIn3Hum, humidityX - 65, humidityY);
                  desiredIn3Hum -= (move);
                  tft.setTextColor(COLOR_MENU_TEXT);
                  drawCentreNumber(desiredIn3Hum, humidityX - 65, humidityY);
                  enableSet = 1;
                }
                move = 0;
              }
              drawStartMessage();
              break;
            case LEDGraphicPosition:
              while (digitalRead(pulse) ) {
                updateData();
                if (move && -move + LEDIntensity >= 0 && -move + LEDIntensity <= LEDMaxIntensity) {
                  tft.setTextColor(COLOR_MENU);
                  drawRightNumber(LEDIntensity, 280, ypos);
                  if (!LEDIntensity && move) {
                    tft.drawRightString("OFF", unitPosition, ypos, textFontSize);
                    tft.setTextColor(COLOR_MENU_TEXT);
                    tft.drawRightString("%", unitPosition, ypos, textFontSize);
                  }
                  LEDIntensity -= 10 * move;
                  analogWrite(ICT, LEDIntensity);
                  tft.setTextColor(COLOR_MENU_TEXT);
                  if (!LEDIntensity && move) {
                    tft.setTextColor(COLOR_MENU);
                    tft.drawRightString("%", unitPosition, ypos, textFontSize);
                    tft.setTextColor(COLOR_MENU_TEXT);
                    tft.drawRightString("OFF", unitPosition, ypos, textFontSize);
                  }
                  else {
                    drawRightNumber(LEDIntensity, 280, ypos);
                  }
                }
                move = 0;
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
                if (move) {
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
                  move = 0;
                }
              }
              break;
            case languageGraphicPosition:
              while (digitalRead(pulse)) {
                updateData();
                if (move) {
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
                  language -= move;
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
                  move = 0;
                }
              }
              settings();
              break;
            case heaterTempGraphicPosition:
              while (digitalRead(pulse) ) {
                updateData();
                if (move && -move + heaterLimitTemp >= 0 && -move + heaterLimitTemp <= heaterMaxTemp) {
                  tft.setTextColor(COLOR_MENU);
                  drawRightNumber(heaterLimitTemp, 280, ypos);
                  if (!heaterLimitTemp && move) {
                    tft.drawRightString("OFF", unitPosition, ypos, textFontSize);
                    tft.setTextColor(COLOR_MENU_TEXT);
                    tft.drawRightString("C", unitPosition, ypos, textFontSize);
                  }
                  heaterLimitTemp -= 5 * move;
                  EEPROM.write(EEPROM_heaterLimitTemp, heaterLimitTemp);
                  tft.setTextColor(COLOR_MENU_TEXT);
                  if (!heaterLimitTemp && move) {
                    tft.setTextColor(COLOR_MENU);
                    tft.drawRightString("C", unitPosition, ypos, textFontSize);
                    tft.setTextColor(COLOR_MENU_TEXT);
                    tft.drawRightString("OFF", unitPosition, ypos, textFontSize);
                  }
                  else {
                    drawRightNumber(heaterLimitTemp, 280, ypos);
                  }
                }
                move = 0;
              }
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
                if (move) {
                  tft.setTextColor(COLOR_MENU);
                  tft.drawFloat(previousTemperature[cornerNTC], 1, valuePosition, ypos, textFontSize);
                  tft.setTextColor(COLOR_MENU_TEXT);
                  diffTemperature[cornerNTC] += move * (0.1);
                  temperature[cornerNTC] += move * (0.1);
                  tft.drawFloat(temperature[cornerNTC], 1, valuePosition, ypos, textFontSize);
                  previousTemperature[cornerNTC] = temperature[cornerNTC];
                  move = 0;
                  EEPROM.write(EEPROM_diffTemperature, int(diffTemperature[cornerNTC] * 10));
                }
              }
              break;
            case humidityCalibrationGraphicPosition:
              while (digitalRead(pulse)) {
                updateData();
                if (move) {
                  tft.setTextColor(COLOR_MENU);
                  tft.drawFloat(humidity, 0, valuePosition, ypos, textFontSize);
                  tft.setTextColor(COLOR_MENU_TEXT);
                  diffHumidity += move;
                  humidity += move;
                  tft.drawFloat(humidity, 0, valuePosition, ypos, textFontSize);
                  move = 0;
                  EEPROM.write(EEPROM_diffHumidity, diffHumidity );
                }
              }
              break;
            case restartCalibrationValuesTempGraphicPosition:
              diffTemperature[cornerNTC] = 0;
              diffHumidity = 0;
              EEPROM.write(EEPROM_diffTemperature, diffTemperature[cornerNTC]);
              EEPROM.write(EEPROM_diffHumidity, diffHumidity);
              readDHT22();
              updateTemp(bothNTC);
              calibrateSensors();
              break;
          }
          break;
      }
      selected = 0;
      tft.fillRect(0, (tft.height() - height_heading) * (bar_pos - 1) / rectangles + height_heading, width_select, (tft.height() - height_heading) / rectangles, WHITE);
      while (!digitalRead(pulse)) {
        updateData();
        if (page != menuPage) {
          back_mode();
        }
      }
      delay(debounceTime);
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
      menu();
    }
    delay((time_back_draw + time_back_wait) / width_back);
  }
  if (millis() - last_pulsed > time_back_wait) {
    drawBack();
  }
  delay(50);
}

