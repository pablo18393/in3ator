void auto_mode() {
  page = 3;
  EEPROM.write(0, page);
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_WHITE);
  for (int i = 0; i <= 5; i++) {
    pos_text[i] = 0;
  }
  pos_text[6] = 1;
  if (language) {
    words[0]  = "";
    words[1]  = "";
    words[2]  = "";
    words[3] = "RAMPA";
    words[4] = "REPETIR";
    words[5] = "VELOCIDAD";
  }
  else {
    words[0]  = "";
    words[1]  = "";
    words[2]  = "";
    words[3] = "RAMP";
    words[4] = "REPEAT";
    words[5] = "SPEED";
  }
  if (keep_am == 0) {
    am_move[0] = 0;
    am_move[1] = 0;
  }
  if (travel_counter || pan_counter) {
    words[6] = "GO";
  }
  else {
    words[6] = "";
  }
  ramp = EEPROM.read(4);
  repeat = EEPROM.read(5);
  am_speed = EEPROM.read(6);
  rectangles = 7;
  drawHeading();
  drawRectangles();
  calculateMin();
  while (digitalRead(pulse) == 0){
    updateData();
  }
  delay(40);
  selectMode();
}

void errorPage() {
  tft.fillScreen(BLACK);
  drawHeading();
  tft.setTextSize(1);
  text_size = 1;
  byte ymess = 50;
  byte cir_rad = 5;
  byte cir_x = 10;
  tft.setTextColor(ILI9341_RED);
  tft.drawCentreString("ERROR", 120, ymess, 4);
  tft.setTextColor(ILI9341_WHITE);
  for (byte i = 0; i <= 2; i++) {
    tft.fillCircle(cir_x, (320 - 50 - 40) * (i + 1) / 3 + 30, cir_rad, WHITE);
    if (language) {
      switch (i) {
        case 0:
          tft.drawString("aumenta distancia", cir_x + cir_rad + 5, (320 - 50 - 40) * (i + 1) / 3 + 20, 4);
          tft.drawString("travelling", cir_x + cir_rad + 5, (320 - 50 - 40) * (i + 1) / 3 + 45, 4);
          break;
        case 1:
          tft.drawString("reduce angulo", cir_x + cir_rad + 5, (320 - 50 - 40) * (i + 1) / 3 + 20, 4);
          tft.drawString("de paneo", cir_x + cir_rad + 5, (320 - 50 - 40) * (i + 1) / 3 + 45, 4);
          break;
        case 2:
          tft.drawString("reduce a velocidad", cir_x + cir_rad + 5, (320 - 50 - 40) * (i + 1) / 3 + 20, 4);
          tft.drawString("numero:      o menor", cir_x + cir_rad + 5, (320 - 50 - 40) * (i + 1) / 3 + 45, 4);
          tft.setTextColor(ILI9341_GREEN);
          drawCentreNumber(d, 128, (320 - 50 - 40) * (i + 1) / 3 + 45);
          break;
      }
    }
    else {
      switch (i) {
        case 0:
          tft.drawString("increase travelling", cir_x + cir_rad + 5, (320 - 50 - 40) * (i + 1) / 3 + 20, 4);
          tft.drawString("distance", cir_x + cir_rad + 5, (320 - 50 - 40) * (i + 1) / 3 + 45, 4);
          break;
        case 1:
          tft.drawString("reduce pan angle", cir_x + cir_rad + 5, (320 - 50 - 40) * (i + 1) / 3 + 20, 4);
          break;
        case 2:
          tft.drawString("reduce speed", cir_x + cir_rad + 5, (320 - 50 - 40) * (i + 1) / 3 + 20, 4);
          tft.drawString("to:      or lower", cir_x + cir_rad + 5, (320 - 50 - 40) * (i + 1) / 3 + 45, 4);
          tft.setTextColor(ILI9341_GREEN);
          drawCentreNumber(d, 64, (320 - 50 - 40) * (i + 1) / 3 + 45);
          break;
      }
    }
  }
  while (digitalRead(pulse)){
    updateData();
  }
  delay(50);
}

