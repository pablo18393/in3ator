void process_page() {
  switch (page) {
    case 1:
      tft.setTextSize(1);
      rectangles = 2;
      print_text = 0;
      drawRectangles();
      drawHeading();
      print_text = 1; //cambiar en todos
      tft.setTextColor(ILI9341_WHITE);
      if (language) {
        tft.drawCentreString("Temperatura", tft.width() / 2, tft.height() / 5 , 4);
      }
      else {
        tft.drawCentreString("Temperature", tft.width() / 2, tft.height() / 5, 4);
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
            //utime_lapse();
          }
          else if (Serial3.peek() == 'H') {
            //CODIGO PARA SOBRECALENTAMIENTO
          }
          else if (Serial3.peek() == 'B') {
            delay(50);
            Serial3.read();
            voltage = read_string();
            current = read_string();
            //drawBattery();
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
          while (!digitalRead(pulse)) {
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
    tft.drawCentreString("GIRA PARA CAMBIAR", tft.width() / 2, 145, 4);
    tft.drawCentreString("VELOCIDAD", tft.width() / 2, 185, 4);
  }
  else {
    tft.drawCentreString("SPIN BUTTON TO", tft.width() / 2, 145, 4);
    tft.drawCentreString("CHANGE SPEED", tft.width() / 2, 185, 4);
  }
}

void drawStopAm() {
  if (language) {
    tft.drawCentreString("PULSA PARA", tft.width() / 2, 220, 4);
    tft.drawCentreString("DETENER", tft.width() / 2, 250, 4);
  }
  else {
    tft.drawCentreString("PRESS BUTTON", tft.width() / 2, 220, 4);
    tft.drawCentreString("TO STOP", tft.width() / 2, 250, 4);
  }
}

void drawStop() {
  if (language) {
    tft.drawCentreString("PULSA BOTON", tft.width() / 2, text_height, 4);
    if (page == 10) {
      tft.drawCentreString("PARA PARADA,", tft.width() / 2, text_height + 30, 4);
    }
    else {
      tft.drawCentreString("PARA FOTO,", tft.width() / 2, text_height + 30, 4);
    }
    tft.drawCentreString("3 SEC SALIR", tft.width() / 2, text_height + 60, 4);
  }
  else {
    tft.drawCentreString("PRESS BUTTON", tft.width() / 2, text_height, 4);
    if (page == 10) {
      tft.drawCentreString("TO PAUSE, HOLD", tft.width() / 2, text_height + 30, 4);
    }
    else {
      tft.drawCentreString("TO SHOOT, HOLD", tft.width() / 2, text_height + 30, 4);
    }
    tft.drawCentreString("3 SEC TO STOP", tft.width() / 2, text_height + 60, 4);
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
      tft.drawCentreString("TIEMPO RESTANTE", tft.width() / 2, 145, 4);
      tft.drawString("H", 80, 185, 4);
      tft.drawString("MIN", 140, 185, 4);
      drawShots();
      drawRemaining();
    }
    else {
      tft.drawCentreString("PAUSADO", tft.width() / 2, 165, 4);
    }
  }
  else {
    if (state_tl) {
      tft.drawCentreString("TIME REMAINING", tft.width() / 2, 145, 4);
      tft.drawString("H", 80, 185, 4);
      tft.drawString("MIN", 140, 185, 4);
      drawShots();
      drawRemaining();
    }
    else {
      tft.drawCentreString("PAUSED", tft.width() / 2, 165, 4);
    }
  }
  tft.setTextColor(ILI9341_WHITE);
  if (language) {
    if (!state_tl) {
      tft.drawCentreString("TIEMPO RESTANTE", tft.width() / 2, 145, 4);
      tft.drawString("H", 80, 185, 4);
      tft.drawString("MIN", 140, 185, 4);
      drawShots();
      drawRemaining();
    }
    else {
      tft.drawCentreString("PAUSADO", tft.width() / 2, 165, 4);
    }
  }
  else {
    if (!state_tl) {
      tft.drawCentreString("TIME REMAINING", tft.width() / 2, 145, 4);
      tft.drawString("H", 80, 185, 4);
      tft.drawString("MIN", 140, 185, 4);
      drawShots();
      drawRemaining();
    }
    else {
      tft.drawCentreString("PAUSED", tft.width() / 2, 165, 4);
    }
  }
  drawRightNumber(stops_done, 110, 90);
}

