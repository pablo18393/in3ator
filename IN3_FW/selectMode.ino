
void selectMode() {
  end = 0;
  if (!(page == 3 || page == 4 || page == 6)) {
    bar_pos = 1;
  }
  else {
    bar_pos = 3;
  }
  selected = 0;
  while (1) {
    ypos = (320 - width_heading) / (2 * rectangles) + (bar_pos - 1) * (320 - width_heading) / (rectangles) + letter_height;
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
      if (!(page == 3 || page == 4 || page == 6) || bar_pos > 3) {
        if (selected) {
          tft.fillRect(0, (320 - width_heading) * (bar_pos - 1) / rectangles + width_heading, width_select, (320 - width_heading) / rectangles, COLOR_CHOSEN);
        }
        else {
          tft.fillRect(0, (320 - width_heading) * (bar_pos - 1) / rectangles + width_heading, width_select, (320 - width_heading) / rectangles, WHITE);
        }
      }
      else {
        if (selected) {
          tft.fillRect(0, width_heading, width_select, (320 - width_heading) * 3 / rectangles, COLOR_CHOSEN);
        }
        else {
          tft.fillRect(0, width_heading, width_select, (320 - width_heading) * 3 / rectangles, WHITE);
        }
      }
      if (!(page == 3 || page == 4 || page == 6)) {
        for (int i = 2; i <= rectangles; i++) {
          tft.fillRect(0, (320 - width_heading) * (i - 1) / rectangles + width_heading - 1, 320, width_space, WHITE); //mejorable
        }
      }
      else {
        for (int i = 4; i <= rectangles; i++) {
          tft.fillRect(0, (320 - width_heading) * (i - 1) / rectangles + width_heading - 1, 320, width_space, WHITE); //mejorable
        }
      }
      while (digitalRead(pulse) == 0) {
        updateData();
        if (page > 1) {
          back_mode();
        }
      }
      switch (page) {
        case 1:
          drawBack();
          switch (bar_pos) {
            case 1:
              chooseMode();
              break;
            case 2:
              auto_mode();
              break;
            case 3:
              time_lapse();
              break;
            case 4:
              stop_motion();
              break;
            case 5:
              settings();
              break;
          }
          break;
        case 30:
          switch (bar_pos) {
            case 1:
              manualMode = 1; //travel
              break;
            case 2:
              manualMode = 0; //pan
              break;
          }
          EEPROM.write(1, manualMode);
          manual_mode();
          break;
        case 3:
          switch (bar_pos) {
            case 3:
              choosePos();
              break;
            case 4:
              while (digitalRead(pulse) ) {
                updateData();
                if (move && -move + ramp >= 10 && -move + ramp >= min_ramp && -move + ramp <= 100) {
                  tft.setTextColor(ILI9341_BLACK);
                  drawRightNumber(ramp, 200, ypos);
                  ramp -= 10 * move;
                  tft.setTextColor(ILI9341_WHITE);
                  drawRightNumber(ramp, 200, ypos);
                  EEPROM.write(4, ramp);
                }
                move = 0;
              }
              break;
            case 5:
              while (digitalRead(pulse) ) {
                updateData();
                if (move) {
                  if (repeat == 0) {
                    tft.setTextColor(ILI9341_BLACK);
                    tft.drawRightString("NO", 235, ypos, 4);
                    tft.setTextColor(ILI9341_WHITE);
                    if (language) {
                      tft.drawRightString("SI", 235, ypos, 4);
                    }
                    else {
                      tft.drawRightString("YES", 235, ypos, 4);
                    }
                  }
                  else {
                    tft.setTextColor(ILI9341_BLACK);
                    if (language) {
                      tft.drawRightString("SI", 235, ypos, 4);
                    }
                    else {
                      tft.drawRightString("YES", 235, ypos, 4);
                    }
                    tft.setTextColor(ILI9341_WHITE);
                    tft.drawRightString("NO", 235, ypos, 4);
                  }
                  repeat = !repeat;
                  move = 0;
                  EEPROM.write(5, repeat);
                }
              }
              break;
            case 6:
              while (digitalRead(pulse) ) {
                updateData();
                if (move && -move + am_speed >= 1 && -move + am_speed <= 40) {
                  tft.setTextColor(ILI9341_BLACK);
                  drawRightNumber(am_speed, 235, ypos);
                  am_speed -= move;
                  tft.setTextColor(ILI9341_WHITE);
                  drawRightNumber(am_speed, 235, ypos);
                  EEPROM.write(6, am_speed);
                }
                calculateMin();
                move = 0;
              }
              break;
            case 7:
              delay(50);
              while (digitalRead(pulse) == 0) {
                updateData();
              }
              if (errorPan) {
                errorPage();
                keep_am = 1;
                auto_mode();
              }
              page = 9;
              calculateMin();
              if (ramp == 100) {
                max_speed = d;
              }
              else if (d < max_speed) {
                max_speed = d;
                max_speed++;
              }
              EEPROM.write(21, max_speed);
              EEPROM.write(0, page);
              Serial3.print("E");
              Serial3.print("t");
              Serial3.print(travel_counter);
              Serial3.print(",p");
              Serial3.print(pan_counter);
              Serial3.print(",O");
              Serial3.print((ramp / ramp_factor));
              Serial3.print(",M");
              Serial3.print("R");
              Serial3.print(repeat);
              Serial3.print("D");
              Serial3.print(delay_start);
              Serial3.print(",");
              if (travel_counter) {
                Serial3.print("X");
                Serial3.print(EEPROM.read(210 + am_speed));
              }
              else {
                Serial3.print("Z");
                Serial3.print(EEPROM.read(170 + am_speed));
              }
              Serial3.print(",");
              delayStart();
              delay(50);
              process_page();
              break;
          }
          selected = 0;
          if (bar_pos > 3) {
            tft.fillRect(0, (320 - width_heading) * (bar_pos - 1) / rectangles + width_heading, width_select, (320 - width_heading) / rectangles, WHITE);
          }
          else {
            updateBigBar();
          }
          while (digitalRead(pulse) == 0) {
            updateData();
            back_mode();
          }
          break;
        case 4:
          switch (bar_pos) {
            case 3:
              choosePos();
              update_min_interval();
              break;
            case 4:
            case 5:
            case 6:
              end = 0;
              while (end < 2) {
                if (bar_pos == 5) {
                  update_min_interval();
                  if (end == 0) {
                    data_min = min_interval / 60000;
                  }
                  else if (data_min == tl_data[2]) {
                    data_min = min_interval / 1000 - tl_data[2] * 60;
                  }
                  else {
                    data_min = 0;
                  }
                }
                else if (end == 1 && bar_pos == 4 && tl_data[0] == 0) {
                  data_min = 1;
                  if (tl_data[1] == 0) {
                    erase_tl_data();
                    tl_data[1] = 1;
                    draw_tl_data();
                  }
                }
                else {
                  data_min = 0;
                }
                if (end == 0 && bar_pos == 6) {
                  data_max = 30;
                }
                else {
                  data_max = 60;
                }
                while (digitalRead(pulse) ) {
                  updateData();
                  if (move && (-move + tl_data[end + 2 * (bar_pos - 4)] >= data_min || move < 0) && (-move + tl_data[end + 2 * (bar_pos - 4)] < data_max || move > 0)) {
                    erase_tl_data();
                    tl_data[end + 2 * (bar_pos - 4)] -= move;
                    draw_tl_data();
                    if (bar_pos == 4 || bar_pos == 6) {
                      update_min_interval();
                    }
                  }
                  if (millis() - blinking > 300) {
                    blinking = millis();
                    if (state_blink == 0) {
                      tft.setTextColor(ILI9341_BLACK);
                      state_blink = !state_blink;
                    }
                    else {
                      tft.setTextColor(ILI9341_WHITE);
                      state_blink = !state_blink;
                    }
                    drawRightNumber(tl_data[end + 2 * (bar_pos - 4)], pos_clip[end], ypos);
                  }
                }
                EEPROM.write(7 + end + 2 * (bar_pos - 4), tl_data[end + 2 * (bar_pos - 4)]);
                tft.setTextColor(ILI9341_WHITE);
                drawRightNumber(tl_data[end + 2 * (bar_pos - 4)], pos_clip[end], ypos);
                end++;
                while (digitalRead(pulse) == 0) {
                  updateData();
                }
                delay(100);
              }
              selected = 0;
              tft.fillRect(0, (320 - width_heading) * (bar_pos - 1) / rectangles + width_heading, width_select, (320 - width_heading) / rectangles, WHITE);
              break;
            case 7:
              delay(50);
              while (digitalRead(pulse) == 0) {
                updateData();
              }
              stops_done = 0;
              stops = (long(tl_data[0]) * 60 + tl_data[1]) * clip_fps; //primer fotograma y ultimo
              Serial3.print("E");
              Serial3.print("t");
              Serial3.print(travel_counter);
              Serial3.print(",p");
              Serial3.print(pan_counter);
              Serial3.print(",TL,");
              Serial3.print(stops); //fotogramas
              Serial3.print(",");
              Serial3.print(tl_speed_travel); //velocidad de movimiento
              Serial3.print(",");
              Serial3.print(tl_speed_pan); //velocidad de movimiento
              Serial3.print(",");
              Serial3.print(long(tl_data[4]) * 60 + tl_data[5]); //tiempo de exposicion en segundos
              Serial3.print(",");
              Serial3.print(long(tl_data[2]) * 60 + tl_data[3]); //interval en segundos
              Serial3.print(",");
              Serial3.print(t_min[0]);
              Serial3.print(",");
              Serial3.print(t_min[1]);
              Serial3.print(",");
              Serial3.print(t_min[2]);
              Serial3.print(",");
              Serial3.print(tl_power);
              Serial3.print(",");
              Serial3.print(delay_start);
              Serial3.print(",");
              page = 10;
              EEPROM.write(0, page);
              delayStart();
              while (Serial3.available()) {
                read_char();
              }
              process_page();
              break;
          }
          selected = 0;
          if (bar_pos > 3) {
            tft.fillRect(0, (320 - width_heading) * (bar_pos - 1) / rectangles + width_heading, width_select, (320 - width_heading) / rectangles, WHITE);
          }
          else {
            updateBigBar();
          }
          break;
        case 5:
          switch (bar_pos) {
            case 1:
              while (digitalRead(pulse) ) {
                updateData();
                if (move && -move + clip_fps >= 5 && -move + clip_fps <= 150) {
                  tft.setTextColor(ILI9341_BLACK);
                  drawRightNumber(clip_fps, 235, ypos);
                  clip_fps -= move;
                  tft.setTextColor(ILI9341_WHITE);
                  drawRightNumber(clip_fps, 235, ypos);
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
                    tft.drawRightString("NO", 235, ypos, 4);
                    tft.setTextColor(ILI9341_WHITE);
                    if (language) {
                      tft.drawRightString("SI", 235, ypos, 4);
                    }
                    else {
                      tft.drawRightString("YES", 235, ypos, 4);
                    }
                  }
                  else {
                    tft.setTextColor(ILI9341_BLACK);
                    if (language) {
                      tft.drawRightString("SI", 235, ypos, 4);
                    }
                    else {
                      tft.drawRightString("YES", 235, ypos, 4);
                    }
                    tft.setTextColor(ILI9341_WHITE);
                    tft.drawRightString("NO", 235, ypos, 4);
                  }
                  auto_lock = !auto_lock;
                  move = 0;
                  EEPROM.write(13, auto_lock);
                }
              }
              break;
            case 3:
              while (digitalRead(pulse)) {
                updateData();
                if (move) {
                  if (tl_power == 0) {
                    tft.setTextColor(ILI9341_BLACK);
                    tft.drawRightString("NO", 235, ypos, 4);
                    tft.setTextColor(ILI9341_WHITE);
                    if (language) {
                      tft.drawRightString("SI", 235, ypos, 4);
                    }
                    else {
                      tft.drawRightString("YES", 235, ypos, 4);
                    }
                  }
                  else {
                    tft.setTextColor(ILI9341_BLACK);
                    if (language) {
                      tft.drawRightString("SI", 235, ypos, 4);
                    }
                    else {
                      tft.drawRightString("YES", 235, ypos, 4);
                    }
                    tft.setTextColor(ILI9341_WHITE);
                    tft.drawRightString("NO", 235, ypos, 4);
                  }
                  tl_power = !tl_power;
                  move = 0;
                  EEPROM.write(15, tl_power);
                }
              }
              break;
            case 4:
              while (digitalRead(pulse) ) {
                updateData();
                if (move && -move + delay_start >= 0 && -move + delay_start <= 60) {
                  tft.setTextColor(ILI9341_BLACK);
                  drawRightNumber(delay_start, 215, ypos);
                  delay_start -= 5 * move;
                  tft.setTextColor(ILI9341_WHITE);
                  drawRightNumber(delay_start, 215, ypos);
                  EEPROM.write(16, delay_start);
                }
                move = 0;
              }
              break;
            case 5:
              while (digitalRead(pulse)) {
                updateData();
                if (move) {
                  if (language == 0) {
                    tft.setTextColor(ILI9341_BLACK);
                    tft.drawRightString("ENG", 235, ypos, 4);
                    tft.setTextColor(ILI9341_WHITE);
                    tft.drawRightString("SPA", 235, ypos, 4);
                  }
                  else if (language == 1) {
                    tft.setTextColor(ILI9341_BLACK);
                    tft.drawRightString("SPA", 235, ypos, 4);
                    tft.setTextColor(ILI9341_WHITE);
                    tft.drawRightString("ENG", 235, ypos, 4);
                  }
                  language = !language;
                  move = 0;
                  EEPROM.write(17, language);
                }
              }
              settings();
              break;
          }
          selected = 0;
          tft.fillRect(0, (320 - width_heading) * (bar_pos - 1) / rectangles + width_heading, width_select, (320 - width_heading) / rectangles, WHITE);
          while (digitalRead(pulse) == 0) {
            updateData();
            back_mode();
          }
          break;
        case 6:
          switch (bar_pos) {
            case 3:
              choosePos();
              break;
            case 4:
              end = 0;
              while (end < 2) {
                if (bar_pos == 5) {
                  update_min_interval();
                  if (end == 0) {
                    data_min = min_interval / 60000;
                  }
                  else if (data_min == tl_data[2]) {
                    data_min = min_interval / 1000 - tl_data[2] * 60;
                  }
                  else {
                    data_min = 0;
                  }
                }
                else if (end == 1 && bar_pos == 4 && tl_data[0] == 0) {
                  data_min = 1;
                  if (tl_data[1] == 0) {
                    erase_tl_data();
                    tl_data[1] = 1;
                    draw_tl_data();
                  }
                }
                else {
                  data_min = 0;
                }
                if (end == 0 && bar_pos == 6) {
                  data_max = 30;
                }
                else {
                  data_max = 60;
                }
                while (digitalRead(pulse) ) {
                  updateData();
                  if (move && (-move + tl_data[end + 2 * (bar_pos - 4)] >= data_min || move < 0) && (-move + tl_data[end + 2 * (bar_pos - 4)] < data_max || move > 0)) {
                    erase_tl_data();
                    tl_data[end + 2 * (bar_pos - 4)] -= move;
                    draw_tl_data();
                  }
                  if (millis() - blinking > 300) {
                    blinking = millis();
                    if (state_blink == 0) {
                      tft.setTextColor(ILI9341_BLACK);
                      state_blink = !state_blink;
                    }
                    else {
                      tft.setTextColor(ILI9341_WHITE);
                      state_blink = !state_blink;
                    }
                    drawRightNumber(tl_data[end + 2 * (bar_pos - 4)], pos_clip[end], ypos);
                  }
                }
                EEPROM.write(7 + end + 2 * (bar_pos - 4), tl_data[end + 2 * (bar_pos - 4)]);
                tft.setTextColor(ILI9341_WHITE);
                drawRightNumber(tl_data[end + 2 * (bar_pos - 4)], pos_clip[end], ypos);
                end++;
                while (digitalRead(pulse) == 0) {
                  updateData();
                }
                delay(100);
              }
              selected = 0;
              tft.fillRect(0, (320 - width_heading) * (bar_pos - 1) / rectangles + width_heading, width_select, (320 - width_heading) / rectangles, WHITE);
              break;
            case 5:
              delay(50);
              while (digitalRead(pulse) == 0) {
                updateData();
              }
              stops_done = 0;
              stops = (long(tl_data[0]) * 60 + tl_data[1]) * clip_fps; //primer fotograma y ultimo
              Serial3.print("E");
              Serial3.print("t");
              Serial3.print(travel_counter);
              Serial3.print(",p");
              Serial3.print(pan_counter);
              Serial3.print(",TM,");
              Serial3.print(stops); //fotogramas
              Serial3.print(",");
              Serial3.print(tl_speed_travel); //velocidad de movimiento
              Serial3.print(",");
              Serial3.print(tl_speed_pan); //velocidad de movimiento
              Serial3.print(",");
              Serial3.print(0); //tiempo de exposicion en segundos
              Serial3.print(",");
              Serial3.print(0); //interval en segundos
              Serial3.print(",");
              Serial3.print(t_min[0]);
              Serial3.print(",");
              Serial3.print(t_min[1]);
              Serial3.print(",");
              Serial3.print(t_min[2]);
              Serial3.print(",");
              Serial3.print(tl_power);
              Serial3.print(",");
              Serial3.print(delay_start);
              Serial3.print(",");

              page = 11;
              EEPROM.write(0, page);
              delayStart();
              while (Serial3.available()) {
                read_char();
              }
              process_page();
              break;
          }
          selected = 0;
          if (bar_pos > 3) {
            tft.fillRect(0, (320 - width_heading) * (bar_pos - 1) / rectangles + width_heading, width_select, (320 - width_heading) / rectangles, WHITE);
          }
          else {
            updateBigBar();
          }
          break;
          break;
      }
    }
  }
}

void eraseBar() {
  //tft.fillRect(0, (320 - width_heading - width_space * (rectangles - 1)) * (bar_pos - 1) / rectangles + width_heading + (bar_pos - 1) * (width_space), width_select, (320 - width_heading - width_space * (rectangles - 1)) / rectangles, COLOR_BAR);
  tft.fillRect(0, (320 - width_heading) * (bar_pos - 1) / rectangles + width_heading, width_select, (320 - width_heading) / rectangles, COLOR_BAR);
}

void updateBar() {
  tft.fillRect(0, (320 - width_heading) * (bar_pos - 1) / rectangles + width_heading, width_select, (320 - width_heading) / rectangles, COLOR_SELECTED);
  if (page == 3 || page == 4 || page == 6) {
    for (int i = 4; i <= rectangles; i++) {
      tft.fillRect(0, (320 - width_heading) * (i - 1) / rectangles + width_heading - 1, 320, width_space, WHITE); //mejorable
    }
  }
  else {
    for (int i = 2; i <= rectangles; i++) {
      tft.fillRect(0, (320 - width_heading) * (i - 1) / rectangles + width_heading - 1, 320, width_space, WHITE); //mejorable
    }
  }
}

void eraseBigBar() {
  tft.fillRect(0, width_heading, width_select, (320 - width_heading) * 3 / rectangles, COLOR_BAR);
}

void updateBigBar() {
  tft.fillRect(0, width_heading, width_select, (320 - width_heading) * 3 / rectangles, WHITE);
  for (int i = 4; i <= rectangles; i++) {
    tft.fillRect(0, (320 - width_heading) * (i - 1) / rectangles + width_heading - 1, 320, width_space, WHITE); //mejorable
  }
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
        Serial3.print("N");
        black();
      }
      if (page == 10) {
        Serial3.print("P");
        state_tl = 0;
        black();
        time_lapse();
      }
      if (page == 11) {
        Serial3.print("P");
        black();
        stop_motion();
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

void drawDistanceNumber() {
  tft.drawNumber(distance_number[end], pos_dist + xpos , ypos, 4);
}

void erase_tl_data() {
  tft.setTextColor(ILI9341_BLACK);
  drawRightNumber(tl_data[end + 2 * (bar_pos - 4)], pos_clip[end], ypos);
}

void draw_tl_data() {
  tft.setTextColor(ILI9341_WHITE);
  drawRightNumber(tl_data[end + 2 * (bar_pos - 4)], pos_clip[end], ypos);
  blinking = millis();
  move = 0;
}

void update_min_interval() {
  stops = long((tl_data[0] * 60 + tl_data[1])) * clip_fps;
  long steps[2];
  steps[0] = travel_counter;
  steps[1] = pan_counter;
  long x = travel_counter;
  long z = pan_counter;
  if (x < 0) {
    x = -x;
  }
  if (z < 0) {
    z = -z;
  }
  long steps_time = max((x * tl_speed_travel) / ((stops - 1) * 1000), (z * tl_speed_pan) / ((stops - 1) * 1000));
  min_interval = (t_min[0] + t_min[1] + t_min[2] + steps_time);
  min_interval *= interval_margin;
  min_interval /= 1000;
  min_interval++;
  min_interval *= 1000;
  min_interval += ((long(tl_data[4]) * 60 + long(tl_data[5]))) * 1000;

  if ((long(tl_data[2]) * 60 + long(tl_data[3])) * 1000 < min_interval) {
    tft.setTextColor(ILI9341_BLACK);
    drawRightNumber(tl_data[2], pos_clip[0], 209);
    drawRightNumber(tl_data[3], pos_clip[1], 209);
    tl_data[2] = min_interval / 60000;
    tl_data[3] = min_interval / 1000 - tl_data[2] * 60;
    EEPROM.write(9, tl_data[2]);
    EEPROM.write(10, tl_data[3]);
    tft.setTextColor(ILI9341_WHITE);
    drawRightNumber(tl_data[2], pos_clip[0], 209);
    drawRightNumber(tl_data[3], pos_clip[1], 209);
  }
}

void delayStart() {
  if (delay_start) {
    tft.fillScreen(BLACK);
    tft.setTextColor(WHITE);
    if (language) {
      tft.drawCentreString("EMPEZANDO EN:", 120, 50, 4);
      tft.drawCentreString("PULSA PARA", 120, 240, 4);
      tft.drawCentreString("COMENZAR", 120, 280, 4);
    }
    else {
      tft.drawCentreString("STARTING IN:", 120, 50, 4);
      tft.drawCentreString("PULSE BUTTON", 120, 240, 4);
      tft.drawCentreString("TO SKIP DELAY", 120, 280, 4);
    }
    tft.setTextSize(2);
    text_size = 2;
    long left = millis();
    byte number_before = delay_start;
    tft.setTextColor(WHITE);
    drawCentreNumber(number_before, 120, 125);
    while (millis() - left < delay_start * 1000 && digitalRead(pulse)) {
      waiting = 1;
      updateData();
      if (number_before != delay_start - (millis() - left) / 1000) {
        tft.setTextColor(BLACK);
        drawCentreNumber(number_before, 120, 125);
        number_before = delay_start - (millis() - left) / 1000;
        tft.setTextColor(WHITE);
        drawCentreNumber(number_before, 120, 125);
      }
    }
    waiting = 0;
    if (digitalRead(pulse) == 0) {
      Serial3.print("U");
      while (digitalRead(pulse) == 0) {
        updateData();
      }
      delay(50);
    }
  }
}

void calculateMin() {
  float speedAm;
  bool only_pan = 0;
  if (travel_counter || pan_counter) {
    if (travel_counter == 0) {
      travel_counter = pan_counter * 100;
      speedAm = EEPROM.read(170 + am_speed) / 100;
      only_pan = 1;
    }
    else {
      speedAm = EEPROM.read(210 + am_speed);
    }
    long minSteps;
    float finalSpeed = 1000;
    bool calculated = 0;
    double accel;
    int w;
    long x = travel_counter;
    long z = pan_counter;
    if (x < 0) {
      x = -x;
    }
    if (z < 0) {
      z = -z;
    }
    for (w = 10; w <= 100 && calculated == 0; w += 10) {
      accel = w;
      accel /= ramp_factor;
      accel /= 25000;
      minSteps = x / 2;
      if (x < 0) {
        minSteps = -minSteps;
      }
      finalSpeed = 1000;
      while (minSteps > 0) {
        finalSpeed /= (accel * finalSpeed + 1);
        minSteps -= (1000 / finalSpeed) * 2;
      }
      if (finalSpeed < speedAm) {
        calculated = 1;
      }
    }
    w -= 10;
    min_ramp = w;
    if (ramp < w) {
      tft.setTextColor(ILI9341_BLACK);
      drawRightNumber(ramp, 200, 168);
      ramp = w;
      EEPROM.write(4, ramp);
      tft.setTextColor(ILI9341_WHITE);
      drawRightNumber(ramp, 200, 168);
    }
    minSteps = x / 2;
    finalSpeed = 1000;
    accel = ramp;
    accel /= ramp_factor;
    accel /= 25000 ;
    while (minSteps > 0) {
      finalSpeed *= accel;
      minSteps -= (1000 / finalSpeed) * 2;
    }
    max_speed = 1;
    if (only_pan) {
      while (finalSpeed < EEPROM.read(170 + max_speed) / 100) {
        max_speed++;
      }
    }
    else {
      while (finalSpeed < EEPROM.read(210 + max_speed)) {
        max_speed++;
      }
    }
    if (pan_counter && only_pan == 0) {
      for (d = 1; d <= 40 && EEPROM.read(210 + d)*x / z > max_speed_pan; d++);
      d--;
      if (EEPROM.read(210 + am_speed)*x / z < max_speed_pan && d) {
        errorPan = 1;
      }
      else {
        errorPan = 0;
      }
    }
    else {
      errorPan = 0;
      d = 41;
    }
  }
  else {
    max_speed = 41;
    min_ramp = 10;
    errorPan = 0;
    d = 41;
  }
  if (only_pan) {
    travel_counter = 0;
  }
}
