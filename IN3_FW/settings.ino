void settings() {
  byte numWords = 5;
  page = 2;
  tft.setTextSize(1);
  tft.setTextColor(COLOR_MENU_TEXT);
  for (int i = 0; i < numWords; i++) {
    pos_text[i] = 0;
  }
  pos_text[4] = 1;
  if (!language) {
    words[0]  = "Auto lock";
    words[1] = "Language";
    words[2] = "Heater temp";
    words[3] = "Fans speed";
    words[4] = "Calibrate";
  }
  else {
    words[0]  = "Auto bloqueo";
    words[1] = "Idioma";
    words[2] = "Temp calefactor";
    words[3] = "Vel Ventiladores";
    words[4] = "Calibrar";
  }
  rectangles = numWords;
  drawGraphicInterface();
  drawHeading();
  updateSensors();
  while (!digitalRead(pulse)) {
    updateData();
  }
  delay(100);
  selectMode();
}


