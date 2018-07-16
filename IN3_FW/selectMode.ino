
void selectMode() {
  bar_pos = 1;
  selected = 0;
  while (1) {
    ypos = (tft.height() - width_heading) / (2 * rectangles) + (bar_pos - 1) * (tft.height() - width_heading) / (rectangles) + letter_height;
    updateData();
    if (move) {
      if (selected == 0) {
        if (move < 0) {
          move++;
          if (bar_pos < rectangles - !enableSet) {
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
                  tft.setTextColor(ILI9341_BLACK);
                  tft.drawFloat(desiredTemp, 1, temperatureX - 65, temperatureY, 4);
                  desiredTemp -= float(move) / 10;
                  tft.setTextColor(ILI9341_WHITE);
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
                  tft.setTextColor(ILI9341_BLACK);
                  drawRightNumber(led_intensity, 280, ypos);
                  if (!led_intensity && move) {
                    tft.drawRightString("OFF", 315, ypos, 4);
                    tft.setTextColor(ILI9341_WHITE);
                    tft.drawRightString("%", 315, ypos, 4);
                  }
                  led_intensity -= 10 * move;
                  analogWrite(HEATER, led_intensity);
                  tft.setTextColor(ILI9341_WHITE);
                  if (!led_intensity && move) {
                    tft.setTextColor(ILI9341_BLACK);
                    tft.drawRightString("%", 315, ypos, 4);
                    tft.setTextColor(ILI9341_WHITE);
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
              while (digitalRead(pulse)) {
                updateData();
                if (move) {
                  if (language == 0) {
                    tft.setTextColor(ILI9341_BLACK);
                    tft.drawRightString("ENG", 315, ypos, 4);
                    tft.setTextColor(ILI9341_WHITE);
                    tft.drawRightString("SPA", 315, ypos, 4);
                  }
                  else if (language == 1) {
                    tft.setTextColor(ILI9341_BLACK);
                    tft.drawRightString("SPA", 315, ypos, 4);
                    tft.setTextColor(ILI9341_WHITE);
                    tft.drawRightString("ENG", 315, ypos, 4);
                  }
                  language = !language;
                }
              }
              menu();
              break;
            case 4:
              process_page();
              break;
          }
          selected = 0;
          tft.fillRect(0, (tft.height() - width_heading) * (bar_pos - 1) / rectangles + width_heading, width_select, (tft.height() - width_heading) / rectangles, WHITE);
          while (digitalRead(pulse) == 0) {
            updateData();
            //back_mode();
          }
          break;
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
      lockPercentage = 0;
      menu();
    }
    delay((time_back_draw + time_back_wait) / width_back);
  }
  if (millis() - last_pulsed > time_back_wait) {
    drawBack();
  }
  delay(50);
}

