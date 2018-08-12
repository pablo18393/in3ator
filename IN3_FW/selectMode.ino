void selectMode() {
  bar_pos = 1;
  selected = 0;
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
        ypos = (tft.height() - width_heading) / (2 * rectangles) + (bar_pos - 1) * (tft.height() - width_heading) / (rectangles) + letter_height;
      }
    }
    if (!digitalRead(pulse)) {
      selected = ! selected;
      if (selected) {
        tft.fillRect(0, (tft.height() - width_heading) * (bar_pos - 1) / rectangles + width_heading, width_select, (tft.height() - width_heading) / rectangles, COLOR_CHOSEN);
      }
      else {
        tft.fillRect(0, (tft.height() - width_heading) * (bar_pos - 1) / rectangles + width_heading, width_select, (tft.height() - width_heading) / rectangles, WHITE);
      }
      for (int i = 2; i <= rectangles; i++) {
        tft.fillRect(0, (tft.height() - width_heading) * (i - 1) / rectangles + width_heading - 1, tft.height(), width_space, WHITE); //mejorable
      }
      while (!digitalRead(pulse)) {
        updateData();
        if (page > 1) {
          back_mode();
        }
      }
      delay(100);
      switch (page) {
        case 0:
          switch (bar_pos) {
            case 1:
              while (digitalRead(pulse)) {
                updateData();
                if (move && -move + desiredIn3Temp >= minTemp && -move + desiredIn3Temp <= maxTemp) {
                  tft.setTextColor(COLOR_MENU);
                  tft.drawFloat(desiredIn3Temp, 1, temperatureX - 65, temperatureY, 4);
                  desiredIn3Temp -= float(move) / 10;
                  tft.setTextColor(COLOR_MENU_TEXT);
                  tft.drawFloat(desiredIn3Temp, 1, temperatureX - 65, temperatureY, 4);
                  enableSet = 1;
                }
                move = 0;
              }
              if (enableSet) {
                menu();
              }
              break;
            case 2:
              while (digitalRead(pulse) ) {
                updateData();
                if (move && -move + LEDIntensity >= 0 && -move + LEDIntensity <= LEDMaxIntensity) {
                  tft.setTextColor(COLOR_MENU);
                  drawRightNumber(LEDIntensity, 280, ypos);
                  if (!LEDIntensity && move) {
                    tft.drawRightString("OFF", 315, ypos, 4);
                    tft.setTextColor(COLOR_MENU_TEXT);
                    tft.drawRightString("%", 315, ypos, 4);
                  }
                  LEDIntensity -= 10 * move;
                  analogWrite(ICT, LEDIntensity);
                  tft.setTextColor(COLOR_MENU_TEXT);
                  if (!LEDIntensity && move) {
                    tft.setTextColor(COLOR_MENU);
                    tft.drawRightString("%", 315, ypos, 4);
                    tft.setTextColor(COLOR_MENU_TEXT);
                    tft.drawRightString("OFF", 315, ypos, 4);
                  }
                  else {
                    drawRightNumber(LEDIntensity, 280, ypos);
                  }
                }
                move = 0;
              }
              break;
            case 3:
              settings();
              break;
            case 4:
              processPage();
              break;
          }
          break;
        case 2:
          switch (bar_pos) {
            case 1:
              while (digitalRead(pulse)) {
                updateData();
                if (move) {
                  tft.setTextColor(COLOR_MENU);
                  if (auto_lock) {
                    if (!language) {
                      tft.drawRightString("YES", 315, ypos, 4);
                    }
                    else {
                      tft.drawRightString("SI", 315, ypos, 4);
                    }
                    tft.setTextColor(COLOR_MENU_TEXT);
                    tft.drawRightString("NO", 315, ypos, 4);
                  }
                  else {
                    tft.setTextColor(COLOR_MENU);
                    tft.drawRightString("NO", 315, ypos, 4);
                    tft.setTextColor(COLOR_MENU_TEXT);
                    if (!language) {
                      tft.drawRightString("YES", 315, ypos, 4);
                    }
                    else {
                      tft.drawRightString("SI", 315, ypos, 4);
                    }
                  }
                  auto_lock = !auto_lock;
                  EEPROM.write(EEPROM_autoLock, auto_lock);
                  move = 0;
                }
              }
              break;
            case 2:
              while (digitalRead(pulse)) {
                updateData();
                if (move) {
                  if (!language) {
                    tft.setTextColor(COLOR_MENU);
                    tft.drawRightString("ENG", 315, ypos, 4);
                    tft.setTextColor(COLOR_MENU_TEXT);
                    tft.drawRightString("SPA", 315, ypos, 4);
                  }
                  else {
                    tft.setTextColor(COLOR_MENU);
                    tft.drawRightString("SPA", 315, ypos, 4);
                    tft.setTextColor(COLOR_MENU_TEXT);
                    tft.drawRightString("ENG", 315, ypos, 4);
                  }
                  language = !language;
                  EEPROM.write(EEPROM_language, language);
                  move = 0;
                }
              }
              settings();
              break;
            case 3:
              while (digitalRead(pulse) ) {
                updateData();
                if (move && -move + heaterLimitTemp >= 0 && -move + heaterLimitTemp <= heaterMaxTemp) {
                  tft.setTextColor(COLOR_MENU);
                  drawRightNumber(heaterLimitTemp, 280, ypos);
                  if (!heaterLimitTemp && move) {
                    tft.drawRightString("OFF", 315, ypos, 4);
                    tft.setTextColor(COLOR_MENU_TEXT);
                    tft.drawRightString("C", 315, ypos, 4);
                  }
                  heaterLimitTemp -= 5 * move;
                  EEPROM.write(EEPROM_heaterLimitTemp, heaterLimitTemp);
                  tft.setTextColor(COLOR_MENU_TEXT);
                  if (!heaterLimitTemp && move) {
                    tft.setTextColor(COLOR_MENU);
                    tft.drawRightString("C", 315, ypos, 4);
                    tft.setTextColor(COLOR_MENU_TEXT);
                    tft.drawRightString("OFF", 315, ypos, 4);
                  }
                  else {
                    drawRightNumber(heaterLimitTemp, 280, ypos);
                  }
                }
                move = 0;
              }
              break;
            case 4:
              while (digitalRead(pulse) ) {
                updateData();
                if (move && -move + fanSpeed >= 0 && -move + fanSpeed <= fanMaxSpeed) {
                  tft.setTextColor(COLOR_MENU);
                  drawRightNumber(fanSpeed, 280, ypos);
                  if (!fanSpeed && move) {
                    tft.drawRightString("OFF", 315, ypos, 4);
                    tft.setTextColor(COLOR_MENU_TEXT);
                    tft.drawRightString("%", 315, ypos, 4);
                  }
                  fanSpeed -= 10 * move;
                  EEPROM.write(EEPROM_fanSpeed, fanSpeed);
                  tft.setTextColor(COLOR_MENU_TEXT);
                  if (!fanSpeed && move) {
                    tft.setTextColor(COLOR_MENU);
                    tft.drawRightString("%", 315, ypos, 4);
                    tft.setTextColor(COLOR_MENU_TEXT);
                    tft.drawRightString("OFF", 315, ypos, 4);
                  }
                  else {
                    drawRightNumber(fanSpeed, 280, ypos);
                  }
                }
                move = 0;
              }
              break;
            case 5:
              calibrateSensors();
              break;
          }
          break;
        case 3:
          switch (bar_pos) {
            case 1:
              while (digitalRead(pulse)) {
                updateData();
                if (move) {
                  tft.setTextColor(COLOR_MENU);
                  tft.drawFloat(temperature[cornerNTC], 1, 245, ypos, 4);
                  tft.setTextColor(COLOR_MENU_TEXT);
                  diffTemperature[cornerNTC] += move * (0.1);
                  temperature[cornerNTC] += move * (0.1);
                  tft.drawFloat(temperature[cornerNTC], 1, 245, ypos, 4);
                  move = 0;
                  EEPROM.write(EEPROM_diffTemperature, int(diffTemperature[cornerNTC] * 10));
                }
              }
              break;
            case 2:
              while (digitalRead(pulse)) {
                updateData();
                if (move) {
                  tft.setTextColor(COLOR_MENU);
                  tft.drawFloat(humidity, 0, 245, ypos, 4);
                  tft.setTextColor(COLOR_MENU_TEXT);
                  diffHumidity += move;
                  humidity += move;
                  tft.drawFloat(humidity, 0, 245, ypos, 4);
                  move = 0;
                  EEPROM.write(EEPROM_diffHumidity, diffHumidity );
                }
              }
              break;
            case 3:
              diffTemperature[cornerNTC] = 0;
              diffHumidity = 0;
              EEPROM.write(EEPROM_diffTemperature, diffTemperature[cornerNTC]);
              EEPROM.write(EEPROM_diffHumidity, diffHumidity);
              updateHumidity();
              updateTemp(bothNTC);
              calibrateSensors();
              break;
          }
          break;
      }
      selected = 0;
      tft.fillRect(0, (tft.height() - width_heading) * (bar_pos - 1) / rectangles + width_heading, width_select, (tft.height() - width_heading) / rectangles, WHITE);
      while (digitalRead(pulse) == 0) {
        updateData();
        back_mode();
      }
    }
  }
}

int getYpos(byte row) {
  return ((tft.height() - width_heading) / (2 * rectangles) + (row - 1) * (tft.height() - width_heading) / (rectangles) + letter_height);
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
    if (!language) {
      helpMessage = "Set desired temperature";
    }
    else {
      helpMessage = "Introduce temperatura";
    }
    tft.drawCentreString(helpMessage, width_select + (tft.width() - width_select) / 2, getYpos(goToProcessRow), 4);
  }
}

void back_mode() {
  delay(100);
  last_pulsed = millis();
  byte back_bar = 0;
  while (!digitalRead(pulse)) {
    updateData();
    if (millis() - last_pulsed > time_back_wait) {
      back_bar++;
      tft.drawLine(width_back - back_bar, 0, width_back - back_bar, width_heading, COLOR_MENU);
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

