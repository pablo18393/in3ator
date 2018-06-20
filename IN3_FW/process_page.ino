void process_page() {
  switch (page) {
    case 9:
      tft.fillScreen(BLACK);
      tft.setTextSize(1);
      text_size = 1;
      tft.setTextColor(ILI9341_WHITE);
      rectangles = 2;
      print_text = 0;
      drawRectangles();
      drawHeading();
      print_text = 1; //cambiar en todos
      if (language) {
        tft.drawCentreString("VELOCIDAD", 120, 80, 4);
      }
      else {
        tft.drawCentreString("SPEED", 120, 80, 4);
      }
      am_speed = EEPROM.read(6);
      drawCentreNumber(am_speed, 120, 120);
      tft.setTextColor(ILI9341_ORANGE);
      //drawSpin();
      drawStopAm();
      state_blink = 1;
      keep_am = 1;
      while (Serial3.peek() != 'F') {
        updateData();
        if (CheckComunication()) {
          instant_read = Serial3.read();
          if (instant_read == 'F') {
            auto_mode();
          }
          else if (instant_read == 'x') {
            black();
            EEPROM.write(0, 1);
            nvic_sys_reset();
          }
        }
        /*
          if (move && abs(-move + am_speed) <= limit_speed && -move + am_speed > 0 && (-move + am_speed) < max_speed) {
          tft.setTextColor(ILI9341_BLACK);
          drawCentreNumber(am_speed, 120, 90);
          am_speed -= move;
          EEPROM.write(6, am_speed);
          tft.setTextColor(ILI9341_WHITE);
          drawCentreNumber(am_speed, 120, 90);
          if (travel_counter) {
            Serial3.print("X");
            Serial3.print(EEPROM.read(210 + am_speed));
          }
          else {
            Serial3.print("Z");
            Serial3.print(EEPROM.read(170 + am_speed));
          }
          }
        */
        move = 0;

        if (digitalRead(pulse) == 0 ) {
          Serial3.print("K");
          delay(10);
          auto_mode();
        }
        if (millis() - blinking > 1000) {
          blinking = millis();
          state_blink = !state_blink;
          if (state_blink) {
            tft.setTextColor(ILI9341_ORANGE);
          }
          else {
            tft.setTextColor(ILI9341_BLACK);
            blinking += 400;
          }
          drawStopAm();
          //drawSpin();
        }
      }
      Serial3.read();
      auto_mode();
      break;
    case 10:
    case 11:
      tft.fillScreen(BLACK);
      tft.setTextSize(1);
      tft.setTextColor(ILI9341_WHITE);
      for (byte j = 0; j < 6; j++) {
        tl_data[j] = EEPROM.read(7 + j);
      }
      clip_fps = EEPROM.read(14);
      stops = (long(EEPROM.read(7)) * 60 + EEPROM.read(8)) * clip_fps; //primer fotograma y ultimo
      if (page == 10) {
        rectangles = 3;
      }
      else {
        rectangles = 2;
      }
      print_text = 0;
      drawRectangles();
      drawHeading();
      print_text = 1; //cambiar en todos
      if (page == 10) {
        shots_height = 50;
        text_height = 315;
      }
      else {
        shots_height = 80;
        text_height = 205;
        blue();
      }
      if (language) {
        tft.drawCentreString("DISPAROS", 120, shots_height , 4);
      }
      else {
        tft.drawCentreString("SHOTS", 120, shots_height, 4);
      }
      tft.drawCentreString("/", 120, shots_height + 40, 4);
      tft.drawNumber(stops, 130, shots_height + 40, 4);
      drawShots();
      if (page == 10) {
        tft.drawString("H", 80, 185, 4);
        tft.drawString("MIN", 140, 185, 4);
        drawRemaining();
        updateTlState();
        if (state_tl) {
          blue();
        }
        else {
          black();
        }
      }
      tft.setTextColor(ILI9341_ORANGE);
      drawStop();
      state_blink = 1;
      allow_shot = 1;
      while (1) {
        updateData();
        if (Serial3.available()) {
          if (Serial3.peek() == 'S') {
            Serial3.read();
            allow_shot = 1;
            if (page == 11) {
              blue();
            }
            tft.setTextColor(ILI9341_BLACK);
            drawShots();
            if (page == 10) {
              drawRemaining();
            }
            stops_done++;
            tft.setTextColor(ILI9341_WHITE);
            drawShots();
            if (page == 10) {
              drawRemaining();
            }
          }
          else if (Serial3.peek() == 'F') {
            Serial3.read();
            keep_am = 1;
            time_lapse();
          }
          else if (Serial3.peek() == 'H') {
            //CODIGO PARA SOBRECALENTAMIENTO
          }
          else if (Serial3.peek() == 'B') {
            delay(50);
            Serial3.read();
            voltage = read_string();
            current = read_string();
            drawBattery();
          }
          else if (Serial3.peek() == 'x') {
            Serial3.read();
            black();
            EEPROM.write(0, 1);
            nvic_sys_reset();
          }
          else {
            Serial3.read();
          }
        }
        if (digitalRead(pulse) ) {
          last_pulsed = millis();
        }
        else {
          if (page == 10) {
            state_tl = !state_tl;
            if (state_tl) {
              blue();
            }
            else {
              black();
            }
            updateTlState();
          }
          delay(50);
          while (digitalRead(pulse) == 0) {
            updateData();
            if (millis() - last_pulsed > 1500 && digitalRead(pulse) == 0) {
              keep_am = 1;
              back_mode();
            }
          }
          if (allow_shot || page == 10) {
            Serial3.print('J');
            allow_shot = 0;
            if (page == 11) {
              green();
            }
          }
          delay(50);
        }
        if (millis() - blinking > 1000) {
          blinking = millis();
          state_blink = !state_blink;
          if (state_blink) {
            tft.setTextColor(ILI9341_ORANGE);
          }
          else {
            tft.setTextColor(ILI9341_BLACK);
            blinking += 400;
          }
          drawStop();
        }
      }
      break;
  }
}

void drawSpin() {
  if (language) {
    tft.drawCentreString("GIRA PARA CAMBIAR", 120, 145, 4);
    tft.drawCentreString("VELOCIDAD", 120, 185, 4);
  }
  else {
    tft.drawCentreString("SPIN BUTTON TO", 120, 145, 4);
    tft.drawCentreString("CHANGE SPEED", 120, 185, 4);
  }
}

void drawStopAm() {
  if (language) {
    tft.drawCentreString("PULSA PARA", 120, 220, 4);
    tft.drawCentreString("DETENER", 120, 250, 4);
  }
  else {
    tft.drawCentreString("PRESS BUTTON", 120, 220, 4);
    tft.drawCentreString("TO STOP", 120, 250, 4);
  }
}

void drawStop() {
  if (language) {
    tft.drawCentreString("PULSA BOTON", 120, text_height, 4);
    if (page == 10) {
      tft.drawCentreString("PARA PARADA,", 120, text_height + 30, 4);
    }
    else {
      tft.drawCentreString("PARA FOTO,", 120, text_height + 30, 4);
    }
    tft.drawCentreString("3 SEC SALIR", 120, text_height + 60, 4);
  }
  else {
    tft.drawCentreString("PRESS BUTTON", 120, text_height, 4);
    if (page == 10) {
      tft.drawCentreString("TO PAUSE, HOLD", 120, text_height + 30, 4);
    }
    else {
      tft.drawCentreString("TO SHOOT, HOLD", 120, text_height + 30, 4);
    }
    tft.drawCentreString("3 SEC TO STOP", 120, text_height + 60, 4);
  }
}

void drawShots() {
  drawRightNumber(stops_done, 110, shots_height + 40);
}

void drawRemaining() {
  byte hours = ((tl_data[2] * 60 + tl_data[3]) * (stops - stops_done)) / 3600;
  byte minutes = ((tl_data[2] * 60 + tl_data[3]) * (stops - stops_done)) / 60 - hours * 60;
  drawRightNumber(hours, 80, 185); //hours
  drawRightNumber(minutes, 140, 185);
}

void updateTlState() {
  tft.setTextColor(ILI9341_BLACK);
  if (language) {
    if (state_tl) {
      tft.drawCentreString("TIEMPO RESTANTE", 120, 145, 4);
      tft.drawString("H", 80, 185, 4);
      tft.drawString("MIN", 140, 185, 4);
      drawShots();
      drawRemaining();
    }
    else {
      tft.drawCentreString("PAUSADO", 120, 165, 4);
    }
  }
  else {
    if (state_tl) {
      tft.drawCentreString("TIME REMAINING", 120, 145, 4);
      tft.drawString("H", 80, 185, 4);
      tft.drawString("MIN", 140, 185, 4);
      drawShots();
      drawRemaining();
    }
    else {
      tft.drawCentreString("PAUSED", 120, 165, 4);
    }
  }
  tft.setTextColor(ILI9341_WHITE);
  if (language) {
    if (!state_tl) {
      tft.drawCentreString("TIEMPO RESTANTE", 120, 145, 4);
      tft.drawString("H", 80, 185, 4);
      tft.drawString("MIN", 140, 185, 4);
      drawShots();
      drawRemaining();
    }
    else {
      tft.drawCentreString("PAUSADO", 120, 165, 4);
    }
  }
  else {
    if (!state_tl) {
      tft.drawCentreString("TIME REMAINING", 120, 145, 4);
      tft.drawString("H", 80, 185, 4);
      tft.drawString("MIN", 140, 185, 4);
      drawShots();
      drawRemaining();
    }
    else {
      tft.drawCentreString("PAUSED", 120, 165, 4);
    }
  }
  drawRightNumber(stops_done, 110, 90);
}

