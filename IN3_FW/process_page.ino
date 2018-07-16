void process_page() {
  page = 1;
  tft.setTextSize(1);
  rectangles = 2;
  print_text = 0;
  drawRectangles();
  drawHeading();
  printLoadingBar();
  print_text = 1; //cambiar en todos
  tft.setTextColor(ILI9341_WHITE);
  if (language) {
    tft.drawCentreString("Temperatura", tft.width() / 2, tft.height() / 5 - 4, 4);
  }
  else {
    tft.drawCentreString("Temperature", tft.width() / 2, tft.height() / 5 - 4, 4);
  }
  tft.drawCentreString("%", tft.width() / 2 + 14, temperatureY, 4);
  tft.setTextColor(ILI9341_ORANGE);
  drawStop();
  state_blink = 1;
  temperatureAtStart = temperature;
  while (!digitalRead(pulse));
  digitalWrite(FAN1, HIGH);
  digitalWrite(FAN1, HIGH);
  digitalWrite(FAN1, HIGH);
  while (1) {
    updateData();
    if (digitalRead(pulse)) {
      last_pulsed = millis();
      delay(100);
    }
    else {
      delay(50);
      while (!digitalRead(pulse)) {
        updateData();
        if (millis() - last_pulsed > 1500 && digitalRead(pulse) == 0) {
          back_mode();
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
    tft.drawCentreString("3 SEC SALIR", tft.width() / 2, text_height + 30, 4);
  }
  else {
    tft.drawCentreString("PRESS BUTTON", tft.width() / 2, text_height, 4);
    tft.drawCentreString("3 SEC TO STOP", tft.width() / 2, text_height + 30, 4);
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

void printLoadingBar() {
  int barWidth = tft.width() / 4 * 3;
  int barHeight = 15;
  int barPosX = tft.width() / 2;
  int barPosY = tft.height() / 3;
  byte barThickness = 3;
  temperatureX = barPosX - barWidth / 2 - 30;
  temperatureY = barPosY + barHeight / 2 + 10;
  updateTemperature();
  tft.drawFloat(desiredTemp, 1, barPosX + barWidth / 2 - 30, temperatureY, 4);
  for (int i = 0; i < barThickness; i++) {
    tft.drawRect(barPosX - barWidth / 2 - i, barPosY - barHeight / 2 - i, barWidth + i * 2, barHeight + i * 2, COLOR_LOADING_BAR);
  }
}

