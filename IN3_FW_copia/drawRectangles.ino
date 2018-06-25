const byte width_select = 7;
const byte width_heading = 34;
const byte width_space = 4;
const byte width_back = 50;
const byte side_gap = 4;
const byte letter_height = 26;
const byte triang = 6;
const byte radius = 12;
const byte circle = 8;
const byte logo = 40;
const byte battery_lenght = 50;
const byte battery_height = 6;
const byte battery_gap = 2;
const byte battery_margin = 20;
const byte battery_round = 4;
const byte arrow_height = 6;
const byte arrow_tail = 5;
int ypos;
bool print_text = 1;

void drawRectangles() {

  tft.fillRect(width_select, width_heading, 320 - width_select, 240 - width_heading, COLOR_MENU);
  if (print_text) {
    tft.fillRect(0, width_heading, width_select, (320 - width_heading) / rectangles, COLOR_SELECTED);
  }
  for (int i = 2; i <= rectangles; i++) {
    tft.fillRect(0, (240 - width_heading) * (i - 1) / rectangles + width_heading, width_select, (240 - width_heading) / rectangles, COLOR_BAR);
    if (!(page == 3 || page == 4 || page == 6) || i > 3) {
      tft.fillRect(0, (240 - width_heading) * (i - 1) / rectangles + width_heading - 1, 320, width_space, WHITE);
    }
    else {
      updateBigBar();
    }
  }
  tft.drawRect(0, 319, width_select, 319, COLOR_MENU);
  if (print_text) {
    tft.setTextSize(1);
    text_size = 1;
    for (int i = 0; i < rectangles; i++) {
      ypos = (240 - width_heading) / (2 * rectangles) + i * (240 - width_heading) / (rectangles) + letter_height;
      if (pos_text[i] == 0) {
        tft.drawString(words[i], width_select + side_gap, ypos, 4);
      }
      else if (pos_text[i] == 1) {
        tft.drawCentreString(words[i], width_select + (240 - width_select) / 2, ypos, 4);
      }
      switch (page) {
        case 3:
          switch (i) {
            case 0:
              drawPos();
              tft.drawRightString("OFF", 315, ypos, 4);
              break;
            case 3:
              ramp = EEPROM.read(4);
              drawRightNumber(ramp, 200, ypos);
              tft.drawRightString("%", 315, ypos, 4);
              break;
            case 4:
              repeat = EEPROM.read(5);
              if (repeat) {
                if (language) {
                  tft.drawRightString("SI", 315, ypos, 4);
                }
                else {
                  tft.drawRightString("YES", 315, ypos, 4);
                }
              }
              else {
                tft.drawRightString("NO", 315, ypos, 4);
              }
              break;
            case 5:
              am_speed = EEPROM.read(6);
              drawRightNumber(am_speed, 315, ypos);
              break;
          }
          break;
        case 4:
        case 6:
          switch (i) {
            case 0:
              drawPos();
              break;
            case 3:
            case 4:
            case 5:
              if (!(page == 6 && i == 4)) {
                tft.drawString("m       s", 165 , ypos, 4);
                for (byte j = 0 ; j <= 1; j++) {
                  drawRightNumber(tl_data[j + 2 * (i - 3)], pos_clip[j], ypos);
                }
              }
              break;
          }
          break;
        case 5:
          switch (i) {
            case 0:
              drawRightNumber(clip_fps, 315, ypos);
              break;
            case 1:
              if (auto_lock) {
                if (language) {
                  tft.drawRightString("SI", 315, ypos, 4);
                }
                else {
                  tft.drawRightString("YES", 315, ypos, 4);
                }
              }
              else {
                tft.drawRightString("NO", 315, ypos, 4);
              }
              break;
            case 2:
              if (tl_power) {
                if (language) {
                  tft.drawRightString("SI", 315, ypos, 4);
                }
                else {
                  tft.drawRightString("YES", 315, ypos, 4);
                }
              }
              else {
                tft.drawRightString("NO", 315, ypos, 4);
              }
              break;
            case 3:
              drawRightNumber(delay_start, 215, ypos);
              tft.drawRightString("s", 315, ypos, 4);
              break;
            case 4:
              if (language == 0) {
                tft.drawRightString("ENG", 315, ypos, 4);
              }
              else if (language == 1) {
                tft.drawRightString("SPA", 315, ypos, 4);
              }
              break;
          }
          break;
      }
    }
  }
}

void drawHeading() {
  for (int i = 0; i <= 1; i++) {
    tft.fillRect(0, 0, 320, width_heading, COLOR_HEADING);
    if (page > 1) {
      drawBack();
    }
  }
  drawBattery();
}

void clearMenu() {
  tft.fillRect(0, width_heading, 240, 320 - width_heading, BLACK);
}

void drawBack() {
  tft.fillRect(0, 0, width_back, width_heading, COLOR_HEADING);
  tft.drawRect(0, 0, width_back, width_heading, BLACK);
  tft.fillTriangle(arrow_height, width_heading / 2, width_back / 2 , arrow_height, width_back / 2, width_heading - arrow_height, COLOR_ARROW);
  tft.fillRect(width_back / 2, width_heading / 2 - arrow_tail, width_back / 2 - arrow_height, arrow_tail, COLOR_ARROW);
  tft.fillRect(width_back / 2, width_heading / 2, width_back / 2 - arrow_height, arrow_tail, COLOR_ARROW);
}

void drawRightNumber(int n, byte x, int i) {
  length = 1;
  for (long k = 10; k <= n; k *= 10) {
    length++;
  }
  tft.drawNumber(n, x - length * 14, i, 4);
}

void drawPos() {
  long pos_color;
  if (keep_am) {
    if (travel_counter) {
      am_move[0] = 1;
      pos_color = GREEN;
    }
    else {
      pos_color = RED;
    }
    tft.fillCircle((240 - width_select) / 5 + 60, width_heading + 10 + 320 / (rectangles), circle, WHITE);
    tft.fillCircle((240 - width_select) / 5 + 60, width_heading + 15 + 320 * 9 / (rectangles * 5), circle, WHITE);
  }
  else {
    pos_color = WHITE;
    tft.fillCircle((240 - width_select) / 5 + 60, width_heading + 10 + 320 / (rectangles), circle, BLACK);
    tft.fillCircle((240 - width_select) / 5 + 60, width_heading + 15 + 320 * 9 / (rectangles * 5), circle, BLACK);
  }
  tft.drawFastHLine((240 - width_select) / 5 - 20, width_heading + 10 + 320 / (rectangles), 40, pos_color);
  tft.fillTriangle((240 - width_select) / 5 - 20 , width_heading + 10 + 320 / (rectangles), (240 - width_select) / 5 - 20 + triang, width_heading + 10 + 320 / (rectangles) - triang, (240 - width_select) / 5 - 20 + triang, width_heading + 10 + 320 / (rectangles) + triang, pos_color);
  tft.fillTriangle((240 - width_select) / 5 + 20 , width_heading + 10 + 320 / (rectangles), (240 - width_select) / 5 + 20 - triang, width_heading + 10 + 320 / (rectangles) - triang, (240 - width_select) / 5 + 20 - triang, width_heading + 10 + 320 / (rectangles) + triang, pos_color);
  if (keep_am) {
    if (pan_counter) {
      pos_color = GREEN;
      am_move[1] = 1;
    }
    else {
      pos_color = RED;
    }
    tft.fillCircle((240 - width_select) / 5 + 160, width_heading + 10 + 320 / (rectangles), circle, WHITE);
    tft.fillCircle((240 - width_select) / 5 + 160, width_heading + 15 + 320 * 9 / (rectangles * 5), circle, WHITE);
  }
  else {
    pos_color = WHITE;
    tft.fillCircle((240 - width_select) / 5 + 160, width_heading + 10 + 320 / (rectangles), circle, BLACK);
    tft.fillCircle((240 - width_select) / 5 + 160, width_heading + 15 + 320 * 9 / (rectangles * 5), circle, BLACK);
  }
  tft.drawCircle((240 - width_select) / 5, width_heading + 15 + 320 * 9 / (rectangles * 5), radius, pos_color);
  tft.fillTriangle((240 - width_select) / 5 - triang / 2, width_heading + 15 + 320 * 9 / (rectangles * 5) + radius, (240 - width_select) / 5 + triang / 2, width_heading + 15 + 320 * 9 / (rectangles * 5) + radius + triang, (240 - width_select) / 5 + triang / 2, width_heading + 15 + 320 * 9 / (rectangles * 5) + radius - triang, pos_color);
  tft.fillTriangle((240 - width_select) / 5 + triang / 2, width_heading + 15 + 320 * 9 / (rectangles * 5) - radius, (240 - width_select) / 5 - triang / 2, width_heading + 15 + 320 * 9 / (rectangles * 5) - radius - triang, (240 - width_select) / 5 - triang / 2, width_heading + 15 + 320 * 9 / (rectangles * 5) - radius + triang, pos_color);

  tft.drawFastHLine((240 - width_select) / 5 + 60, width_heading + 320 * 3 / (rectangles * 6), 100, WHITE);
  tft.fillCircle((240 - width_select) / 5 + 60, width_heading + 320 * 3 / (rectangles * 6), triang - 2, WHITE);
  tft.fillCircle((240 - width_select) / 5 + 160, width_heading + 320 * 3 / (rectangles * 6), triang - 2, WHITE);
  tft.fillTriangle((240 - width_select) / 5 + 93, width_heading + 320 * 3 / (rectangles * 6) - triang, (240 - width_select) / 5 + 93, width_heading + 320 * 3 / (rectangles * 6) + triang, (240 - width_select) / 5 + 93 + triang, width_heading + 320 * 3 / (rectangles * 6), WHITE);
  tft.fillTriangle((240 - width_select) / 5 + 126, width_heading + 320 * 3 / (rectangles * 6) - triang, (240 - width_select) / 5 + 126, width_heading + 320 * 3 / (rectangles * 6) + triang, (240 - width_select) / 5 + 126 + triang, width_heading + 320 * 3 / (rectangles * 6), WHITE);

  tft.drawCircle((240 - width_select) / 5 + 60, width_heading + 10 + 320 / (rectangles), circle, WHITE);
  tft.drawCircle((240 - width_select) / 5 + 60, width_heading + 15 + 320 * 9 / (rectangles * 5), circle, WHITE);
  tft.drawCircle((240 - width_select) / 5 + 160, width_heading + 10 + 320 / (rectangles), circle, WHITE);
  tft.drawCircle((240 - width_select) / 5 + 160, width_heading + 15 + 320 * 9 / (rectangles * 5), circle, WHITE);
}

void printTravel() {
  if (am_move[0]) {
    tft.drawFastHLine((240 - width_select) / 5 - 20, width_heading + 10 + 320 / (rectangles), 40, GREEN);
    tft.fillTriangle((240 - width_select) / 5 - 20 , width_heading + 10 + 320 / (rectangles), (240 - width_select) / 5 - 20 + triang, width_heading + 10 + 320 / (rectangles) - triang, (240 - width_select) / 5 - 20 + triang, width_heading + 10 + 320 / (rectangles) + triang, GREEN);
    tft.fillTriangle((240 - width_select) / 5 + 20 , width_heading + 10 + 320 / (rectangles), (240 - width_select) / 5 + 20 - triang, width_heading + 10 + 320 / (rectangles) - triang, (240 - width_select) / 5 + 20 - triang, width_heading + 10 + 320 / (rectangles) + triang, GREEN);
  }
  else {
    tft.drawFastHLine((240 - width_select) / 5 - 20, width_heading + 10 + 320 / (rectangles), 40, RED);
    tft.fillTriangle((240 - width_select) / 5 - 20 , width_heading + 10 + 320 / (rectangles), (240 - width_select) / 5 - 20 + triang, width_heading + 10 + 320 / (rectangles) - triang, (240 - width_select) / 5 - 20 + triang, width_heading + 10 + 320 / (rectangles) + triang, RED);
    tft.fillTriangle((240 - width_select) / 5 + 20 , width_heading + 10 + 320 / (rectangles), (240 - width_select) / 5 + 20 - triang, width_heading + 10 + 320 / (rectangles) - triang, (240 - width_select) / 5 + 20 - triang, width_heading + 10 + 320 / (rectangles) + triang, RED);
  }
}

void printPan() {
  if (am_move[1]) {
    tft.drawCircle((240 - width_select) / 5, width_heading + 15 + 320 * 9 / (rectangles * 5), radius, GREEN);
    tft.fillTriangle((240 - width_select) / 5 - triang / 2, width_heading + 15 + 320 * 9 / (rectangles * 5) + radius, (240 - width_select) / 5 + triang / 2, width_heading + 15 + 320 * 9 / (rectangles * 5) + radius + triang, (240 - width_select) / 5 + triang / 2, width_heading + 15 + 320 * 9 / (rectangles * 5) + radius - triang, GREEN);
    tft.fillTriangle((240 - width_select) / 5 + triang / 2, width_heading + 15 + 320 * 9 / (rectangles * 5) - radius, (240 - width_select) / 5 - triang / 2, width_heading + 15 + 320 * 9 / (rectangles * 5) - radius - triang, (240 - width_select) / 5 - triang / 2, width_heading + 15 + 320 * 9 / (rectangles * 5) - radius + triang, GREEN);
  }
  else {
    tft.drawCircle((240 - width_select) / 5, width_heading + 15 + 320 * 9 / (rectangles * 5), radius, RED);
    tft.fillTriangle((240 - width_select) / 5 - triang / 2, width_heading + 15 + 320 * 9 / (rectangles * 5) + radius, (240 - width_select) / 5 + triang / 2, width_heading + 15 + 320 * 9 / (rectangles * 5) + radius + triang, (240 - width_select) / 5 + triang / 2, width_heading + 15 + 320 * 9 / (rectangles * 5) + radius - triang, RED);
    tft.fillTriangle((240 - width_select) / 5 + triang / 2, width_heading + 15 + 320 * 9 / (rectangles * 5) - radius, (240 - width_select) / 5 - triang / 2, width_heading + 15 + 320 * 9 / (rectangles * 5) - radius - triang, (240 - width_select) / 5 - triang / 2, width_heading + 15 + 320 * 9 / (rectangles * 5) - radius + triang, RED);
  }
}

void drawRespective(byte c) {
  unsigned int respectiveColor;
  if (c == 1) {
    respectiveColor = BLACK;
  }
  else {
    respectiveColor = WHITE;
  }
  switch (end) {
    case 0:
      if (c == 2) {
        tft.fillCircle((240 - width_select) / 5 + 60, width_heading + 10 + 320 / (rectangles), circle, respectiveColor);
      }
      tft.drawCircle((240 - width_select) / 5 + 60, width_heading + 10 + 320 / (rectangles), circle, respectiveColor);
      tft.drawFastHLine((240 - width_select) / 5 - 20, width_heading + 10 + 320 / (rectangles), 40, respectiveColor);
      tft.fillTriangle((240 - width_select) / 5 - 20 , width_heading + 10 + 320 / (rectangles), (240 - width_select) / 5 - 20 + triang, width_heading + 10 + 320 / (rectangles) - triang, (240 - width_select) / 5 - 20 + triang, width_heading + 10 + 320 / (rectangles) + triang, respectiveColor);
      tft.fillTriangle((240 - width_select) / 5 + 20 , width_heading + 10 + 320 / (rectangles), (240 - width_select) / 5 + 20 - triang, width_heading + 10 + 320 / (rectangles) - triang, (240 - width_select) / 5 + 20 - triang, width_heading + 10 + 320 / (rectangles) + triang, respectiveColor);
      break;
    case 1:
      if (c == 2) {
        tft.fillCircle((240 - width_select) / 5 + 60, width_heading + 15 + 320 * 9 / (rectangles * 5), circle, respectiveColor);
      }
      tft.drawCircle((240 - width_select) / 5 + 60, width_heading + 15 + 320 * 9 / (rectangles * 5), circle, respectiveColor);
      tft.drawCircle((240 - width_select) / 5, width_heading + 15 + 320 * 9 / (rectangles * 5), radius, respectiveColor);
      tft.fillTriangle((240 - width_select) / 5 - triang / 2, width_heading + 15 + 320 * 9 / (rectangles * 5) + radius, (240 - width_select) / 5 + triang / 2, width_heading + 15 + 320 * 9 / (rectangles * 5) + radius + triang, (240 - width_select) / 5 + triang / 2, width_heading + 15 + 320 * 9 / (rectangles * 5) + radius - triang, respectiveColor);
      tft.fillTriangle((240 - width_select) / 5 + triang / 2, width_heading + 15 + 320 * 9 / (rectangles * 5) - radius, (240 - width_select) / 5 - triang / 2, width_heading + 15 + 320 * 9 / (rectangles * 5) - radius - triang, (240 - width_select) / 5 - triang / 2, width_heading + 15 + 320 * 9 / (rectangles * 5) - radius + triang, respectiveColor);
      break;
    case 2:
      if (c == 2) {
        tft.fillCircle((240 - width_select) / 5 + 160, width_heading + 10 + 320 / (rectangles), circle, respectiveColor);
      }
      tft.drawCircle((240 - width_select) / 5 + 160, width_heading + 10 + 320 / (rectangles), circle, respectiveColor);
      tft.drawFastHLine((240 - width_select) / 5 - 20, width_heading + 10 + 320 / (rectangles), 40, respectiveColor);
      tft.fillTriangle((240 - width_select) / 5 - 20 , width_heading + 10 + 320 / (rectangles), (240 - width_select) / 5 - 20 + triang, width_heading + 10 + 320 / (rectangles) - triang, (240 - width_select) / 5 - 20 + triang, width_heading + 10 + 320 / (rectangles) + triang, respectiveColor);
      tft.fillTriangle((240 - width_select) / 5 + 20 , width_heading + 10 + 320 / (rectangles), (240 - width_select) / 5 + 20 - triang, width_heading + 10 + 320 / (rectangles) - triang, (240 - width_select) / 5 + 20 - triang, width_heading + 10 + 320 / (rectangles) + triang, respectiveColor);
      break;
    case 3:
      if (c == 2) {
        tft.fillCircle((240 - width_select) / 5 + 160, width_heading + 15 + 320 * 9 / (rectangles * 5), circle, respectiveColor);
      }
      tft.drawCircle((240 - width_select) / 5 + 160, width_heading + 15 + 320 * 9 / (rectangles * 5), circle, respectiveColor);
      tft.drawCircle((240 - width_select) / 5, width_heading + 15 + 320 * 9 / (rectangles * 5), radius, respectiveColor);
      tft.fillTriangle((240 - width_select) / 5 - triang / 2, width_heading + 15 + 320 * 9 / (rectangles * 5) + radius, (240 - width_select) / 5 + triang / 2, width_heading + 15 + 320 * 9 / (rectangles * 5) + radius + triang, (240 - width_select) / 5 + triang / 2, width_heading + 15 + 320 * 9 / (rectangles * 5) + radius - triang, respectiveColor);
      tft.fillTriangle((240 - width_select) / 5 + triang / 2, width_heading + 15 + 320 * 9 / (rectangles * 5) - radius, (240 - width_select) / 5 - triang / 2, width_heading + 15 + 320 * 9 / (rectangles * 5) - radius - triang, (240 - width_select) / 5 - triang / 2, width_heading + 15 + 320 * 9 / (rectangles * 5) - radius + triang, respectiveColor);
      break;
  }
}

void loadLogo() {
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_BLACK);
  tft.drawCentreString("I N 3", 120, 100, 4);
}

void drawBattery() {
  battery_lines = ((voltage + (current - current_0) * 2.2 - battery_min) * battery_div) / (battery_max - battery_min);
  battery_lines = max(battery_lines, 0);
  battery_lines = min(battery_lines, battery_div);
  if (battery_lines) {
    battery_warning = 0;
  }
  else if (battery_warning == 0) {
    battery_warning = 1;
    draw_battery_warning();
  }
  if (battery_lines > battery_lines0) {
    if (battery_sure / battery_percentage > voltage) {
      battery_lines = battery_lines0;
    }
    else {
      battery_sure = voltage;
    }
  }
  else {
    battery_sure = voltage;
  }
  if (battery_warning_0 && battery_warning == 0) {
    for (int l = 0; l <= 3; l++) {
      tft.drawRoundRect(240 - battery_lenght - battery_height + l, battery_height, battery_lenght, width_heading - 2 * battery_height, battery_round, COLOR_HEADING);
      tft.drawRoundRect(240 - battery_lenght - battery_height, battery_height + l, battery_lenght, width_heading - 2 * battery_height, battery_round, COLOR_HEADING);
    }
  }
  if (battery_lines0 != battery_lines || page0 != page || first_draw_battery) {
    first_draw_battery = 0;
    tft.fillRect(240 - battery_lenght - battery_height, battery_height, battery_lenght, width_heading - 2 * battery_height, COLOR_HEADING);
    tft.drawRoundRect(240 - battery_lenght - battery_height, battery_height, battery_lenght, width_heading - 2 * battery_height, battery_round, COLOR_BATTERY);
    for (byte b = 0; b < battery_lines; b++) {
      tft.fillRect(240 - battery_lenght - battery_height + (b + 1)*battery_gap + b * (battery_lenght - (battery_div + 1)*battery_gap) / battery_div, battery_height + battery_gap, (battery_lenght - (battery_div + 1)*battery_gap) / battery_div, width_heading - 2 * battery_height - 2 * battery_gap, COLOR_BATTERY_LEFT);
    }
  }
  battery_lines0 = battery_lines;
  battery_warning_0 = battery_warning;
  page0 = page;
}

void draw_battery_warning() {
  if (millis() - last_battery_blink > 500) {
    if (battery_blink) {
      for (int l = 0; l <= 3; l++) {
        tft.drawRoundRect(240 - battery_lenght - battery_height + l, battery_height, battery_lenght, width_heading - 2 * battery_height, battery_round, ILI9341_RED);
        tft.drawRoundRect(240 - battery_lenght - battery_height, battery_height + l, battery_lenght, width_heading - 2 * battery_height, battery_round, ILI9341_RED);
      }
    }
    else {
      for (int l = 0; l <= 3; l++) {
        tft.drawRoundRect(240 - battery_lenght - battery_height + l, battery_height, battery_lenght, width_heading - 2 * battery_height, battery_round, COLOR_BATTERY);
        tft.drawRoundRect(240 - battery_lenght - battery_height, battery_height + l, battery_lenght, width_heading - 2 * battery_height, battery_round, COLOR_BATTERY);
      }
    }
    battery_blink = !battery_blink;
    last_battery_blink = millis();
  }
}

