void selectMode() {
  bool enableSetProcess;
  bar_pos = 1;
  selected = 0;
  while (1) {
    ypos = (tft.height() - width_heading) / (2 * rectangles) + (bar_pos - 1) * (tft.height() - width_heading) / (rectangles) + letter_height;
    updateData();
    if (move) {
      if (selected == 0) {
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
                if (move && -move + desiredTemp >= 15 && -move + desiredTemp <= 45) {
                  tft.setTextColor(COLOR_MENU);
                  tft.drawFloat(desiredTemp, 1, temperatureX - 65, temperatureY, 4);
                  desiredTemp -= float(move) / 10;
                  tft.setTextColor(COLOR_MENU_TEXT);
                  tft.drawFloat(desiredTemp, 1, temperatureX - 65, temperatureY, 4);
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
                if (move && -move + led_intensity >= 0 && -move + led_intensity <= 100) {
                  tft.setTextColor(COLOR_MENU);
                  drawRightNumber(led_intensity, 280, ypos);
                  if (!led_intensity && move) {
                    tft.drawRightString("OFF", 315, ypos, 4);
                    tft.setTextColor(COLOR_MENU_TEXT);
                    tft.drawRightString("%", 315, ypos, 4);
                  }
                  led_intensity -= 10 * move;
                  analogWrite(HEATER, led_intensity);
                  tft.setTextColor(COLOR_MENU_TEXT);
                  if (!led_intensity && move) {
                    tft.setTextColor(COLOR_MENU);
                    tft.drawRightString("%", 315, ypos, 4);
                    tft.setTextColor(COLOR_MENU_TEXT);
                    tft.drawRightString("OFF", 315, ypos, 4);
                  }
                  else {
                    drawRightNumber(led_intensity, 280, ypos);
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
                  EEPROM.write(1, auto_lock);
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
                  EEPROM.write(2, language);
                  move = 0;
                }
              }
              settings();
              break;
            case 3:
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
                  tft.drawFloat(temperature, 1, 245, ypos, 4);
                  tft.setTextColor(COLOR_MENU_TEXT);
                  diffTemperature += move * (0.1);
                  temperature += move * (0.1);
                  tft.drawFloat(temperature, 1, 245, ypos, 4);
                  move = 0;
                  EEPROM.write(3, int(diffTemperature * 10));
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
                  EEPROM.write(4, diffHumidity );
                }
              }
              break;
            case 3:
              diffTemperature = 0;
              diffHumidity = 0;
              EEPROM.write(3, diffTemperature);
              EEPROM.write(4, diffHumidity);
              updateHumidity();
              updateTemp();
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

void back_mode() {
  delay(100);
  last_pulsed = millis();
  byte back_bar = 0;
  while (!digitalRead(pulse)) {
    updateData();
    if (millis() - last_pulsed > time_back_wait) {
      back_bar++;
      tft.drawLine(width_back - back_bar, 0, width_back - back_bar, width_heading, BLACK);
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

