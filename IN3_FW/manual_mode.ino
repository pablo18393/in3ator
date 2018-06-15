
void manual_mode() {
  page = 2;
  EEPROM.write(0, page);
  drawHeading();
  clearMenu();
  tft.setTextColor(ILI9341_WHITE);
  tft.setTextSize(1);
  manualMode = EEPROM.read(1);
  if (manualMode) {
    tft.drawCentreString("TRAVELLING", width_select + (240 - width_select) / 2, 265, 4);
  }
  else {
    if (language) {
      tft.drawCentreString("PANEO/F-FOCUS", width_select + (240 - width_select) / 2, 265, 4);
    }
    else {
      tft.drawCentreString("PAN/F-FOCUS", width_select + (240 - width_select) / 2, 265, 4);
    }
  }
  tft.setTextSize(2);
  text_size = 2;
  drawCentreNumber(abs(manual_speed), 120, height_letter);
  red();
  tft.drawRect(120 - height / 2, height_bar, height, height, WHITE);
  tft.drawRect(120 - height / 2 - separation - rect_length - 1, height_bar - 1, rect_length + 2, height + 2, WHITE);
  tft.drawRect(120 + height / 2 + separation - 1, height_bar - 1, rect_length + 2, height + 2, WHITE);
  blinking = millis();
  if (manual_speed == 0) {
    manual_state = 0;
    moved = 0;
  }
  am_move[0] = 0;
  am_move[1] = 0;
  keep_am = 0;
  travel_counter = 0;
  pan_counter = 0;
  Serial3.print("W");
  Serial3.print("E");

  while (1) {
    updateData();
    if (manual_state == 1) {
      if (moved == 0 && move) {
        moved = 1;
        tft.fillRect(120 - height / 2, height_bar, height, height, BLACK);
        if (move < 0) {
          tft.fillTriangle(120 - height / 2 + 2, height_bar + 2, 120 - height / 2 + 2, height_bar + height - 4, 120 + height / 2 - 4, height_bar + height / 2 - 2, GREEN);
        }
        else {
          tft.fillTriangle( 120 + height / 2 - 4, height_bar + 2, 120 + height / 2 - 4, height_bar + height - 4, 120 - height / 2 + 2, height_bar + height / 2 - 2, GREEN);
        }
        green();
      }
      if (manual_speed - move == 0 && move) {
        moved = 0;
        tft.drawRect(120 - height / 2 - separation - 2, height_bar, 2, height, BLACK);
        tft.fillRect(120 - height / 2 + 1, height_bar + 1, height - 2, height - 2, BLUE);
        blue();
      }
      if (move && abs(-move + manual_speed) <= limit_speed) {
        tft.setTextColor(ILI9341_BLACK);
        drawCentreNumber(abs(manual_speed), 120, height_letter);
        manual_speed -= move;
        tft.setTextColor(ILI9341_WHITE);
        drawCentreNumber(abs(manual_speed), 120, height_letter);

        if (manual_speed < 0) {
          if (move < 0) {
            tft.drawRect(120 - height / 2 - separation + 2 * manual_speed - 2, height_bar, 2, height, BLACK);
          }
          else {
            tft.drawRect(120 - height / 2 - separation + 2 * manual_speed, height_bar, 2 , height, WHITE);
          }
        }
        else {
          if (move < 0) {
            tft.drawRect(120 + height / 2 + separation + 2 * manual_speed - 2, height_bar, 2, height, WHITE);
          }
          else {
            tft.drawRect(120 + height / 2 + separation + 2 * manual_speed, height_bar, 2, height, BLACK);
          }
        }
        if (manual_speed == 0) {
          if (stopped == 0) {
            if (manualMode) {
              Serial3.print("EO");
              Serial3.print(acceleration_stop);
              Serial3.print(",");
              Serial3.print("Px");
            }
            else {
              Serial3.print("Pz");
            }
            speed = min_speed_travel;
          }
        }
        else {
          if (manualMode) {
            stopped = 0;
            if (abs(manual_speed) == 1) {
              Serial3.print("EO");
              Serial3.print(acceleration);
              Serial3.print(",");
            }
            Serial3.print("X");
            if (manual_speed < 0) {
              Serial3.print(-int(EEPROM.read(210 - manual_speed)));
            }
            else {
              Serial3.print(EEPROM.read(210 + manual_speed));
            }
          }
          else {
            Serial3.print("Z");
            if (manual_speed < 0) {
              Serial3.print(-int(EEPROM.read(170 - manual_speed) ));
            }
            else {
              Serial3.print(EEPROM.read(170 + manual_speed) );
            }
          }
          Serial3.print(",");
        }
        move = 0;
      }
    }

    if (millis() - blinking > time_blink && manual_state == 0) {
      blinking = millis();
      if (state_blink == 0) {
        tft.fillRect(120 - height / 2 , height_bar , height , height , RED);
        state_blink = 1;
      }
      else {
        tft.fillRect(120 - height / 2 , height_bar, height, height, BLACK);
        state_blink = 0;
      }
    }
    if (digitalRead(pulse) == 0) {
      manual_state = ! manual_state;
      if (manual_state == 1) {
        blue();
        tft.fillRect(120 - height / 2 , height_bar , height , height, BLUE);
      }
      else {
        tft.setTextColor(ILI9341_BLACK);
        drawCentreNumber(abs(manual_speed), 120, height_letter);
        if (stopped == 0) {
          if (manualMode) {
            Serial3.print("EO");
            Serial3.print(acceleration_stop);
            Serial3.print(",");
            if (manual_speed != 0) {
              Serial3.print("Px");
            }
          }
        }
        if (manualMode == 0) {
          Serial3.print("Pz");
        }
        manual_speed = 0;
        tft.setTextColor(ILI9341_WHITE);
        drawCentreNumber(abs(manual_speed), 120, height_letter);
        tft.fillRect(120 - height / 2 , height_bar , height , height , BLACK);
        tft.fillRect(120 - height / 2 - separation - rect_length, height_bar, rect_length, height, BLACK);
        tft.fillRect(120 + height / 2 + separation, height_bar, rect_length, height, BLACK);
        red();
        stopped = 1;
        encoderpos[counter] = 0;
        oldPosition = 0;
        newPosition = 0;
        move = 0;
        moved = 0;
        speed = min_speed_travel;
      }
      back_mode();
    }
  }
}

void chooseMode() {
  page = 30;
  EEPROM.write(0, page);
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_WHITE);
  for (int i = 0; i <= 2; i++) {
    pos_text[i] = 0;
  }

  words[0]  = "TRAVELING";
  if (language) {
    words[1] = "PANEO/F-FOCUS";
  }
  else {
    words[1] = "PAN/F-FOCUS";
  }
  rectangles = 2;
  drawHeading();
  drawRectangles();
  selectMode();
}

void drawCentreNumber(int n, byte x, int i) {
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
