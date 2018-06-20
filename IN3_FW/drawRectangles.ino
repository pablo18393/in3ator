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

  tft.fillRect(width_select, width_heading, tft.width() - width_select, tft.height() - width_heading, COLOR_MENU);
  if (print_text) {
    tft.fillRect(0, width_heading, width_select, (tft.height() - width_heading) / rectangles, COLOR_SELECTED);
  }
  for (int i = 2; i <= rectangles; i++) {
    tft.fillRect(0, (tft.height() - width_heading) * (i - 1) / rectangles + width_heading, width_select, (tft.height() - width_heading) / rectangles, COLOR_BAR);
    if (!(page == 3 || page == 4 || page == 6) || i > 3) {
      tft.fillRect(0, (tft.height() - width_heading) * (i - 1) / rectangles + width_heading - 1, tft.width(), width_space, WHITE);
    }
    else {
      updateBigBar();
    }
  }
  tft.drawRect(0, tft.height()-1, width_select, tft.height()-1, COLOR_MENU);
  if (print_text) {
    tft.setTextSize(1);
    text_size = 1;
    for (int i = 0; i < rectangles; i++) {
      ypos = (tft.height() - width_heading) / (2 * rectangles) + i * (tft.height() - width_heading) / (rectangles) + letter_height;
      if (pos_text[i] == 0) {
        tft.drawString(words[i], width_select + side_gap, ypos, 4);
      }
      else if (pos_text[i] == 1) {
        tft.drawCentreString(words[i], width_select + (tft.width() - width_select) / 2, ypos, 4);
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
    tft.fillRect(0, 0, tft.height(), width_heading, COLOR_HEADING);
    if (page > 1) {
      drawBack();
    }
  }
  drawBattery();
}

void clearMenu() {
  tft.fillRect(0, width_heading, tft.width(), tft.height() - width_heading, BLACK);
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
    tft.fillCircle((tft.width() - width_select) / 5 + 60, width_heading + 10 + tft.height() / (rectangles), circle, WHITE);
    tft.fillCircle((tft.width() - width_select) / 5 + 60, width_heading + 15 + tft.height() * 9 / (rectangles * 5), circle, WHITE);
  }
  else {
    pos_color = WHITE;
    tft.fillCircle((tft.width() - width_select) / 5 + 60, width_heading + 10 + tft.height() / (rectangles), circle, BLACK);
    tft.fillCircle((tft.width() - width_select) / 5 + 60, width_heading + 15 + tft.height() * 9 / (rectangles * 5), circle, BLACK);
  }
  tft.drawFastHLine((tft.width() - width_select) / 5 - 20, width_heading + 10 + tft.height() / (rectangles), 40, pos_color);
  tft.fillTriangle((tft.width() - width_select) / 5 - 20 , width_heading + 10 + tft.height() / (rectangles), (tft.width() - width_select) / 5 - 20 + triang, width_heading + 10 + tft.height() / (rectangles) - triang, (tft.width() - width_select) / 5 - 20 + triang, width_heading + 10 + tft.height() / (rectangles) + triang, pos_color);
  tft.fillTriangle((tft.width() - width_select) / 5 + 20 , width_heading + 10 + tft.height() / (rectangles), (tft.width() - width_select) / 5 + 20 - triang, width_heading + 10 + tft.height() / (rectangles) - triang, (tft.width() - width_select) / 5 + 20 - triang, width_heading + 10 + tft.height() / (rectangles) + triang, pos_color);
  if (keep_am) {
    if (pan_counter) {
      pos_color = GREEN;
      am_move[1] = 1;
    }
    else {
      pos_color = RED;
    }
    tft.fillCircle((tft.width() - width_select) / 5 + 160, width_heading + 10 + tft.height() / (rectangles), circle, WHITE);
    tft.fillCircle((tft.width() - width_select) / 5 + 160, width_heading + 15 + tft.height() * 9 / (rectangles * 5), circle, WHITE);
  }
  else {
    pos_color = WHITE;
    tft.fillCircle((tft.width() - width_select) / 5 + 160, width_heading + 10 + tft.height() / (rectangles), circle, BLACK);
    tft.fillCircle((tft.width() - width_select) / 5 + 160, width_heading + 15 + tft.height() * 9 / (rectangles * 5), circle, BLACK);
  }
  tft.drawCircle((tft.width() - width_select) / 5, width_heading + 15 + tft.height() * 9 / (rectangles * 5), radius, pos_color);
  tft.fillTriangle((tft.width() - width_select) / 5 - triang / 2, width_heading + 15 + tft.height() * 9 / (rectangles * 5) + radius, (tft.width() - width_select) / 5 + triang / 2, width_heading + 15 + tft.height() * 9 / (rectangles * 5) + radius + triang, (tft.width() - width_select) / 5 + triang / 2, width_heading + 15 + tft.height() * 9 / (rectangles * 5) + radius - triang, pos_color);
  tft.fillTriangle((tft.width() - width_select) / 5 + triang / 2, width_heading + 15 + tft.height() * 9 / (rectangles * 5) - radius, (tft.width() - width_select) / 5 - triang / 2, width_heading + 15 + tft.height() * 9 / (rectangles * 5) - radius - triang, (tft.width() - width_select) / 5 - triang / 2, width_heading + 15 + tft.height() * 9 / (rectangles * 5) - radius + triang, pos_color);

  tft.drawFastHLine((tft.width() - width_select) / 5 + 60, width_heading + tft.height() * 3 / (rectangles * 6), 100, WHITE);
  tft.fillCircle((tft.width() - width_select) / 5 + 60, width_heading + tft.height() * 3 / (rectangles * 6), triang - 2, WHITE);
  tft.fillCircle((tft.width() - width_select) / 5 + 160, width_heading + tft.height() * 3 / (rectangles * 6), triang - 2, WHITE);
  tft.fillTriangle((tft.width() - width_select) / 5 + 93, width_heading + tft.height() * 3 / (rectangles * 6) - triang, (tft.width() - width_select) / 5 + 93, width_heading + tft.height() * 3 / (rectangles * 6) + triang, (tft.width() - width_select) / 5 + 93 + triang, width_heading + tft.height() * 3 / (rectangles * 6), WHITE);
  tft.fillTriangle((tft.width() - width_select) / 5 + 126, width_heading + tft.height() * 3 / (rectangles * 6) - triang, (tft.width() - width_select) / 5 + 126, width_heading + tft.height() * 3 / (rectangles * 6) + triang, (tft.width() - width_select) / 5 + 126 + triang, width_heading + tft.height() * 3 / (rectangles * 6), WHITE);

  tft.drawCircle((tft.width() - width_select) / 5 + 60, width_heading + 10 + tft.height() / (rectangles), circle, WHITE);
  tft.drawCircle((tft.width() - width_select) / 5 + 60, width_heading + 15 + tft.height() * 9 / (rectangles * 5), circle, WHITE);
  tft.drawCircle((tft.width() - width_select) / 5 + 160, width_heading + 10 + tft.height() / (rectangles), circle, WHITE);
  tft.drawCircle((tft.width() - width_select) / 5 + 160, width_heading + 15 + tft.height() * 9 / (rectangles * 5), circle, WHITE);
}

void printTravel() {
  if (am_move[0]) {
    tft.drawFastHLine((tft.width() - width_select) / 5 - 20, width_heading + 10 + tft.height() / (rectangles), 40, GREEN);
    tft.fillTriangle((tft.width() - width_select) / 5 - 20 , width_heading + 10 + tft.height() / (rectangles), (tft.width() - width_select) / 5 - 20 + triang, width_heading + 10 + tft.height() / (rectangles) - triang, (tft.width() - width_select) / 5 - 20 + triang, width_heading + 10 + tft.height() / (rectangles) + triang, GREEN);
    tft.fillTriangle((tft.width() - width_select) / 5 + 20 , width_heading + 10 + tft.height() / (rectangles), (tft.width() - width_select) / 5 + 20 - triang, width_heading + 10 + tft.height() / (rectangles) - triang, (tft.width() - width_select) / 5 + 20 - triang, width_heading + 10 + tft.height() / (rectangles) + triang, GREEN);
  }
  else {
    tft.drawFastHLine((tft.width() - width_select) / 5 - 20, width_heading + 10 + tft.height() / (rectangles), 40, RED);
    tft.fillTriangle((tft.width() - width_select) / 5 - 20 , width_heading + 10 + tft.height() / (rectangles), (tft.width() - width_select) / 5 - 20 + triang, width_heading + 10 + tft.height() / (rectangles) - triang, (tft.width() - width_select) / 5 - 20 + triang, width_heading + 10 + tft.height() / (rectangles) + triang, RED);
    tft.fillTriangle((tft.width() - width_select) / 5 + 20 , width_heading + 10 + tft.height() / (rectangles), (tft.width() - width_select) / 5 + 20 - triang, width_heading + 10 + tft.height() / (rectangles) - triang, (tft.width() - width_select) / 5 + 20 - triang, width_heading + 10 + tft.height() / (rectangles) + triang, RED);
  }
}

void printPan() {
  if (am_move[1]) {
    tft.drawCircle((tft.width() - width_select) / 5, width_heading + 15 + tft.height() * 9 / (rectangles * 5), radius, GREEN);
    tft.fillTriangle((tft.width() - width_select) / 5 - triang / 2, width_heading + 15 + tft.height() * 9 / (rectangles * 5) + radius, (tft.width() - width_select) / 5 + triang / 2, width_heading + 15 + tft.height() * 9 / (rectangles * 5) + radius + triang, (tft.width() - width_select) / 5 + triang / 2, width_heading + 15 + tft.height() * 9 / (rectangles * 5) + radius - triang, GREEN);
    tft.fillTriangle((tft.width() - width_select) / 5 + triang / 2, width_heading + 15 + tft.height() * 9 / (rectangles * 5) - radius, (tft.width() - width_select) / 5 - triang / 2, width_heading + 15 + tft.height() * 9 / (rectangles * 5) - radius - triang, (tft.width() - width_select) / 5 - triang / 2, width_heading + 15 + tft.height() * 9 / (rectangles * 5) - radius + triang, GREEN);
  }
  else {
    tft.drawCircle((tft.width() - width_select) / 5, width_heading + 15 + tft.height() * 9 / (rectangles * 5), radius, RED);
    tft.fillTriangle((tft.width() - width_select) / 5 - triang / 2, width_heading + 15 + tft.height() * 9 / (rectangles * 5) + radius, (tft.width() - width_select) / 5 + triang / 2, width_heading + 15 + tft.height() * 9 / (rectangles * 5) + radius + triang, (tft.width() - width_select) / 5 + triang / 2, width_heading + 15 + tft.height() * 9 / (rectangles * 5) + radius - triang, RED);
    tft.fillTriangle((tft.width() - width_select) / 5 + triang / 2, width_heading + 15 + tft.height() * 9 / (rectangles * 5) - radius, (tft.width() - width_select) / 5 - triang / 2, width_heading + 15 + tft.height() * 9 / (rectangles * 5) - radius - triang, (tft.width() - width_select) / 5 - triang / 2, width_heading + 15 + tft.height() * 9 / (rectangles * 5) - radius + triang, RED);
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
        tft.fillCircle((tft.width() - width_select) / 5 + 60, width_heading + 10 + tft.height() / (rectangles), circle, respectiveColor);
      }
      tft.drawCircle((tft.width() - width_select) / 5 + 60, width_heading + 10 + tft.height() / (rectangles), circle, respectiveColor);
      tft.drawFastHLine((tft.width() - width_select) / 5 - 20, width_heading + 10 + tft.height() / (rectangles), 40, respectiveColor);
      tft.fillTriangle((tft.width() - width_select) / 5 - 20 , width_heading + 10 + tft.height() / (rectangles), (tft.width() - width_select) / 5 - 20 + triang, width_heading + 10 + tft.height() / (rectangles) - triang, (tft.width() - width_select) / 5 - 20 + triang, width_heading + 10 + tft.height() / (rectangles) + triang, respectiveColor);
      tft.fillTriangle((tft.width() - width_select) / 5 + 20 , width_heading + 10 + tft.height() / (rectangles), (tft.width() - width_select) / 5 + 20 - triang, width_heading + 10 + tft.height() / (rectangles) - triang, (tft.width() - width_select) / 5 + 20 - triang, width_heading + 10 + tft.height() / (rectangles) + triang, respectiveColor);
      break;
    case 1:
      if (c == 2) {
        tft.fillCircle((tft.width() - width_select) / 5 + 60, width_heading + 15 + tft.height() * 9 / (rectangles * 5), circle, respectiveColor);
      }
      tft.drawCircle((tft.width() - width_select) / 5 + 60, width_heading + 15 + tft.height() * 9 / (rectangles * 5), circle, respectiveColor);
      tft.drawCircle((tft.width() - width_select) / 5, width_heading + 15 + tft.height() * 9 / (rectangles * 5), radius, respectiveColor);
      tft.fillTriangle((tft.width() - width_select) / 5 - triang / 2, width_heading + 15 + tft.height() * 9 / (rectangles * 5) + radius, (tft.width() - width_select) / 5 + triang / 2, width_heading + 15 + tft.height() * 9 / (rectangles * 5) + radius + triang, (tft.width() - width_select) / 5 + triang / 2, width_heading + 15 + tft.height() * 9 / (rectangles * 5) + radius - triang, respectiveColor);
      tft.fillTriangle((tft.width() - width_select) / 5 + triang / 2, width_heading + 15 + tft.height() * 9 / (rectangles * 5) - radius, (tft.width() - width_select) / 5 - triang / 2, width_heading + 15 + tft.height() * 9 / (rectangles * 5) - radius - triang, (tft.width() - width_select) / 5 - triang / 2, width_heading + 15 + tft.height() * 9 / (rectangles * 5) - radius + triang, respectiveColor);
      break;
    case 2:
      if (c == 2) {
        tft.fillCircle((tft.width() - width_select) / 5 + 160, width_heading + 10 + tft.height() / (rectangles), circle, respectiveColor);
      }
      tft.drawCircle((tft.width() - width_select) / 5 + 160, width_heading + 10 + tft.height() / (rectangles), circle, respectiveColor);
      tft.drawFastHLine((tft.width() - width_select) / 5 - 20, width_heading + 10 + tft.height() / (rectangles), 40, respectiveColor);
      tft.fillTriangle((tft.width() - width_select) / 5 - 20 , width_heading + 10 + tft.height() / (rectangles), (tft.width() - width_select) / 5 - 20 + triang, width_heading + 10 + tft.height() / (rectangles) - triang, (tft.width() - width_select) / 5 - 20 + triang, width_heading + 10 + tft.height() / (rectangles) + triang, respectiveColor);
      tft.fillTriangle((tft.width() - width_select) / 5 + 20 , width_heading + 10 + tft.height() / (rectangles), (tft.width() - width_select) / 5 + 20 - triang, width_heading + 10 + tft.height() / (rectangles) - triang, (tft.width() - width_select) / 5 + 20 - triang, width_heading + 10 + tft.height() / (rectangles) + triang, respectiveColor);
      break;
    case 3:
      if (c == 2) {
        tft.fillCircle((tft.width() - width_select) / 5 + 160, width_heading + 15 + tft.height() * 9 / (rectangles * 5), circle, respectiveColor);
      }
      tft.drawCircle((tft.width() - width_select) / 5 + 160, width_heading + 15 + tft.height() * 9 / (rectangles * 5), circle, respectiveColor);
      tft.drawCircle((tft.width() - width_select) / 5, width_heading + 15 + tft.height() * 9 / (rectangles * 5), radius, respectiveColor);
      tft.fillTriangle((tft.width() - width_select) / 5 - triang / 2, width_heading + 15 + tft.height() * 9 / (rectangles * 5) + radius, (tft.width() - width_select) / 5 + triang / 2, width_heading + 15 + tft.height() * 9 / (rectangles * 5) + radius + triang, (tft.width() - width_select) / 5 + triang / 2, width_heading + 15 + tft.height() * 9 / (rectangles * 5) + radius - triang, respectiveColor);
      tft.fillTriangle((tft.width() - width_select) / 5 + triang / 2, width_heading + 15 + tft.height() * 9 / (rectangles * 5) - radius, (tft.width() - width_select) / 5 - triang / 2, width_heading + 15 + tft.height() * 9 / (rectangles * 5) - radius - triang, (tft.width() - width_select) / 5 - triang / 2, width_heading + 15 + tft.height() * 9 / (rectangles * 5) - radius + triang, respectiveColor);
      break;
  }
}

void loadLogo() {
  tft.setTextSize(2);
  tft.setTextColor(ILI9341_BLACK);
  tft.drawCentreString("I N 3", 120, 100, 4);
}

void drawBattery() {

  page0 = page;
}

void draw_battery_warning() {

}

