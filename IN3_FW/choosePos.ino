
void choosePos() {
  am_move[0] = 0;
  am_move[1] = 0;
  keep_am = 0;
  travel_counter = 0;
  pan_counter = 0;
  EEPROMWriteLong(22, 0);
  EEPROMWriteLong(24, 0);
  drawPos();
  end = 0;
  manualMode = 1;
  Serial3.print("N");
  Serial3.print("O");
  Serial3.print(acceleration);
  Serial3.print(",");
  Serial3.print("E");
  Serial3.print("Ix");
  while (Serial3.available()) {
    Serial3.read();
  }
  while (end < 4) {
    manual_state = 0;
    moved = 0;
    state_blink = 1;
    manual_speed = 0;
    blue();
    while (digitalRead(pulse) ) {
      updateData();
      if (moved == 0 && move) {
        moved = 1;
        green();
      }
      if (manual_speed - move == 0 && move) {
        moved = 0;
        blue();
      }
      if (move && abs(-move + manual_speed) <= limit_speed) {
        manual_speed -= move;
        if (abs(manual_speed) == 0) {
          if (manualMode) {
            Serial3.print("Px");
          }
          else {
            Serial3.print("Pz");
          }
          speed = min_speed_travel;
        }
        else {
          if (manualMode) {
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
      if (millis() - blinking > time_blink) {
        blinking = millis();
        drawRespective(state_blink);
        state_blink = !state_blink;
      }
    }
    if (manualMode) {
      Serial3.print("Px");
    }
    else {
      Serial3.print("Pz");
    }
    drawRespective(2);
    back_mode();
    if (end == 0) {
      Serial3.print("Ix");
    }
    end++;
    manualMode = !manualMode;
    switch (end) {
      case 2:
        Serial3.print("Iz");
        break;
      case 3:
        Serial3.print("Fx");
        delay(50);
        if (CheckComunication()) {
          if (Serial3.peek() == '-') {
            Serial3.read();
            travel_counter = read_string();
            travel_counter = -travel_counter;
          }
          else {
            travel_counter = read_string();
          }
          if (travel_counter) {
            am_move[0] = 1;
          }
          else {
            am_move[0] = 0;
          }
        }
        else {
          //error
        }
        printTravel();
        break;
      case 4:
        Serial3.print("Fz");
        delay(50);
        if (CheckComunication()) {
          if (Serial3.peek() == '-') {
            Serial3.read();
            pan_counter = read_string();
            pan_counter = -pan_counter;
          }
          else {
            pan_counter = read_string();
          }
          if (pan_counter) {
            am_move[1] = 1;
          }
          else {
            am_move[1] = 0;
          }
        }
        else {
          //error
        }
        printPan();
        break;
    }
  }
  EEPROMWriteLong(22, travel_counter);
  EEPROMWriteLong(24, pan_counter);
  keep_am = 1;
  if (am_move[0] || am_move[1]) {
    tft.setTextColor(ILI9341_WHITE);
  }
  else {
    tft.setTextColor(ILI9341_BLACK);
  }
  tft.setTextSize(1);
  if (page == 6) {
    tft.drawCentreString("GO", 123, 282, 4);
  }
  else {
    tft.drawCentreString("GO", 123, 291, 4);
  }
  Serial3.print("Y");
  black();
  if (page == 3) {
    calculateMin();
  }
  else if (page == 4) {
    update_min_interval();
  }
}



