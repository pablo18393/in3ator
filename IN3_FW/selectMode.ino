
void selectMode() {
  end = 0;
  bar_pos = 1;
  selected = 0;
  while (1) {
    ypos = (tft.height() - width_heading) / (2 * rectangles) + (bar_pos - 1) * (tft.height() - width_heading) / (rectangles) + letter_height;
    updateData();
    if (move) {
      if (selected == 0) {
        if (move < 0) {
          move++;
          if (!(page == 3 || page == 4 || page == 6)) {
            if (bar_pos < rectangles) {
              eraseBar();
              bar_pos++;
              updateBar();
            }
          }
          else if (bar_pos == 3) {
            eraseBigBar();
            bar_pos++;
            updateBar();
          }
          else if (bar_pos < rectangles) {
            if (page == 3 || page == 4) {
              bar_limit = 6;
            }
            else {
              bar_limit = 4;
            }
            if (bar_pos != bar_limit || (am_move[0] || am_move[1])) {
              eraseBar();
              bar_pos++;
              updateBar();
            }
          }
        }
        else {
          move --;
          if (!(page == 3 || page == 4 || page == 6)) {
            if (bar_pos > 1) {
              eraseBar();
              bar_pos--;
              updateBar();
            }
          }
          else {
            if (bar_pos > 4) {
              eraseBar();
              bar_pos--;
              updateBar();
            }
            else if (bar_pos == 4) {
              eraseBar();
              bar_pos = 3;
              updateBigBar();
            }
          }
        }
      }
    }
    if (digitalRead(pulse) == 0) {
      selected = ! selected;
      if (selected) {
        tft.fillRect(0, (tft.height() - width_heading) * (bar_pos - 1) / rectangles + width_heading, width_select, (tft.height() - width_heading) / rectangles, COLOR_CHOSEN);
      }
      else {
        tft.fillRect(0, (tft.height() - width_heading) * (bar_pos - 1) / rectangles + width_heading, width_select, (tft.height() - width_heading) / rectangles, WHITE);
      }
      if (!(page == 3 || page == 4 || page == 6)) {
        for (int i = 2; i <= rectangles; i++) {
          tft.fillRect(0, (tft.height() - width_heading) * (i - 1) / rectangles + width_heading - 1, tft.height(), width_space, WHITE); //mejorable
        }
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
              while (digitalRead(pulse) ) {
                updateData();
                if (move && -move + clip_fps >= 5 && -move + clip_fps <= 150) {
                  tft.setTextColor(ILI9341_BLACK);
                  drawRightNumber(clip_fps, 315, ypos);
                  clip_fps -= move;
                  tft.setTextColor(ILI9341_WHITE);
                  drawRightNumber(clip_fps, 315, ypos);
                  EEPROM.write(14, clip_fps);
                }
                move = 0;
              }
              break;
            case 2:
              while (digitalRead(pulse) ) {
                updateData();
                if (move) {
                  if (auto_lock == 0) {
                    tft.setTextColor(ILI9341_BLACK);
                    tft.drawRightString("NO", 315, ypos, 4);
                    tft.setTextColor(ILI9341_WHITE);
                    if (language) {
                      tft.drawRightString("SI", 315, ypos, 4);
                    }
                    else {
                      tft.drawRightString("YES", 315, ypos, 4);
                    }
                  }
                  else {
                    tft.setTextColor(ILI9341_BLACK);
                    if (language) {
                      tft.drawRightString("SI", 315, ypos, 4);
                    }
                    else {
                      tft.drawRightString("YES", 315, ypos, 4);
                    }
                    tft.setTextColor(ILI9341_WHITE);
                    tft.drawRightString("NO", 315, ypos, 4);
                  }
                  auto_lock = !auto_lock;
                  move = 0;
                  EEPROM.write(13, auto_lock);
                }
              }
              break;
            case 3:
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
                  analogWrite(HEATER,led_intensity);
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
            case 4:
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

void eraseBar() {
  //tft.fillRect(0, (tft.height() - width_heading - width_space * (rectangles - 1)) * (bar_pos - 1) / rectangles + width_heading + (bar_pos - 1) * (width_space), width_select, (tft.height() - width_heading - width_space * (rectangles - 1)) / rectangles, COLOR_BAR);
  tft.fillRect(0, (tft.height() - width_heading) * (bar_pos - 1) / rectangles + width_heading, width_select, (tft.height() - width_heading) / rectangles, COLOR_BAR);
}

void updateBar() {
  tft.fillRect(0, (tft.height() - width_heading) * (bar_pos - 1) / rectangles + width_heading, width_select, (tft.height() - width_heading) / rectangles, COLOR_SELECTED);
  for (int i = 2; i <= rectangles; i++) {
    tft.fillRect(0, (tft.height() - width_heading) * (i - 1) / rectangles + width_heading - 1, tft.height(), width_space, WHITE); //mejorable
  }
}

void eraseBigBar() {
  tft.fillRect(0, width_heading, width_select, (tft.height() - width_heading) * 3 / rectangles, COLOR_BAR);
}

void updateBigBar() {

}

void back_mode() {
  delay(50);
  last_pulsed = millis();
  byte back_bar = 0;
  while (digitalRead(pulse) == 0) {
    updateData();
    if (millis() - last_pulsed > time_back_wait) {
      back_bar++;
      tft.drawLine(width_back - back_bar, 0, width_back - back_bar, width_heading, BLACK);
    }
    if (back_bar == width_back) {
      if (page > 1 || page < 5) {
        Serial.print("N");
        black();
      }
      if (page == 10) {
        Serial.print("P");
        state_tl = 0;
        black();
        //time_lapse();
      }
      if (page == 11) {
        Serial.print("P");
        black();
        //stop_motion();
      }
      page = 1;
      EEPROM.write(0, page);
      menu();
    }
    delay((time_back_draw + time_back_wait) / width_back);
  }
  if (millis() - last_pulsed > time_back_wait) {
    drawBack();
  }
  delay(50);
}
