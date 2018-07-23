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
  if (!page) {
    tft.fillRect(width_select, width_heading, tft.width() - width_select, tft.height() - width_heading, COLOR_MENU);
  }
  else {
    tft.fillRect(0, width_heading, tft.width(), tft.height() - width_heading, COLOR_MENU);
  }
  if (print_text) {
    tft.fillRect(0, width_heading, width_select, (tft.height() - width_heading) / rectangles, COLOR_SELECTED);
  }
  for (int i = 2; i <= rectangles; i++) {
    tft.fillRect(0, (tft.height() - width_heading) * (i - 1) / rectangles + width_heading, width_select, (tft.height() - width_heading) / rectangles, COLOR_BAR);
    tft.fillRect(0, (tft.height() - width_heading) * (i - 1) / rectangles + width_heading - 1, tft.width(), width_space, WHITE);
  }
  tft.drawRect(0, tft.height() - 1, width_select, tft.height() - 1, COLOR_MENU);
  if (print_text) {
    tft.setTextSize(1);
    text_size = 1;
    for (int i = 0; i < rectangles; i++) {
      ypos = (tft.height() - width_heading) / (2 * rectangles) + i * (tft.height() - width_heading) / (rectangles) + letter_height;
      if (pos_text[i] == 0) {
        tft.drawString(words[i], width_select + side_gap, ypos, 4);
      }
      else if (pos_text[i]) {
        tft.drawCentreString(words[i], width_select + (tft.width() - width_select) / 2, ypos, 4);
      }
      switch (page) {
        case 0:
          switch (i) {
            case 0:
              drawTemperature();
              if (enableSet) {
                tft.drawFloat(desiredTemp, 1, temperatureX - 65, temperatureY, 4);
              }
              break;
            case 4: //implement in the future with humidity
              tft.drawRightString("/", 262, ypos, 4);
              drawCentreNumber(humidity, humidityX, ypos);
              tft.drawRightString("%", 315, ypos, 4);
              break;
            case 1:
              if (led_intensity) {
                drawRightNumber(led_intensity, 280, ypos);
                tft.drawRightString("%", 315, ypos, 4);
              }
              else {
                tft.drawRightString("OFF", 315, ypos, 4);
              }
              break;
          }
          break;
        case 2:
          switch (i) {
            case 0:
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
            case 1:
              if (language) {
                tft.drawRightString("SPA", 315, ypos, 4);
              }
              else {
                tft.drawRightString("ENG", 315, ypos, 4);
              }
              break;
          }
          break;
      }
    }
  }
}

void setVariablesPosition() {
  humidityX = tft.width() - 43;
  humidityY = width_heading / 5;
  temperatureX = tft.width() - 79;
  temperatureY = 51;
}

void drawHeading() {
  for (int i = 0; i <= 1; i++) {
    tft.fillRect(0, 0, tft.height(), width_heading, COLOR_HEADING);
    if (page) {
      drawBack();
    }
  }
  tft.setTextColor(COLOR_MENU);
  tft.drawCentreString("In3ator   hum:      %", tft.width() - 110, width_heading / 5, 4);
}

void eraseBar() {
  tft.fillRect(0, (tft.height() - width_heading) * (bar_pos - 1) / rectangles + width_heading, width_select, (tft.height() - width_heading) / rectangles, COLOR_BAR);
}

void updateBar() {
  tft.fillRect(0, (tft.height() - width_heading) * (bar_pos - 1) / rectangles + width_heading, width_select, (tft.height() - width_heading) / rectangles, COLOR_SELECTED);
  for (int i = 2; i <= rectangles; i++) {
    tft.fillRect(0, (tft.height() - width_heading) * (i - 1) / rectangles + width_heading - 1, tft.height(), width_space, WHITE); //mejorable
  }
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

void drawRightNumber(int n, int x, int i) {
  length = 1;
  for (long k = 10; k <= n; k *= 10) {
    length++;
  }
  tft.drawNumber(n, x - length * 14, i, 4);
}

void loadLogo() {
  tft.setTextSize(2);
  tft.setTextColor(COLOR_MENU);
  tft.drawCentreString("I N 3", tft.width() / 2, 100, 4);
  for (int i = 0; i < backlight_intensity; i++) {
    analogWrite(SCREENBACKLIGHT, i);
    delay(20);
  }
}

void drawTemperature() {
  tft.drawRightString("/", 240, ypos, 4);
  tft.drawRightString("C", 315, ypos, 4);
  updateSensors();
}

void drawCentreNumber(int n, int x, int i) {
  length = 1;
  for (long k = 10; k <= n; k *= 10) {
    length++;
  }
  if (text_size == 2) {
    tft.drawNumber(n, x - length * 27, i, 6);
  }
  else {
    tft.drawNumber(n, x - length * text_size * 7, i, 4);
  }
}

void drawStop() {
  if (language) {
    tft.drawCentreString("PULSA BOTON", tft.width() / 2, text_height, 4);
    tft.drawCentreString("3 SEC SALIR", tft.width() / 2, text_height + 30, 4);
  }
  else {
    tft.drawCentreString("PRESS BUTTON", tft.width() / 2, text_height, 4);
    tft.drawCentreString("3 SEC TO STOP", tft.width() / 2, text_height + 30, 4);
  }
}

void printLoadingBar() {
  barWidth = tft.width() / 4 * 3;
  barHeight = 15;
  barPosX = tft.width() / 2;
  barPosY = tft.height() / 3;
  barThickness = 3;
  temperatureX = barPosX - barWidth / 2 - 30;
  temperatureY = barPosY + barHeight / 2 + 10;
  updateSensors();
  tft.drawFloat(desiredTemp, 1, barPosX + barWidth / 2 - 30, temperatureY, 4);
  for (int i = 1; i <= barThickness; i++) {
    tft.drawRect(barPosX - barWidth / 2 - i, barPosY - barHeight / 2 - i, barWidth + i * 2, barHeight + i * 2, COLOR_FRAME_BAR);
  }
}
