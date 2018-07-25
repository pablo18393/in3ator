void menu() {
  page = 0;
  byte numWords = 4;
  setVariablesPosition();
  tft.setTextSize(1);
  tft.setTextColor(COLOR_MENU_TEXT);
  for (int i = 0; i < numWords; i++) {
    pos_text[i] = 0;
  }
  pos_text[2] = 1;
  pos_text[3] = 1;
  if (language) {
    words[0]  = "Temperatura";
    words[1] = "LEDS";
    words[2] = "Configuracion";
    if (enableSet) {
      words[3] = "Empezar";
    }
    else {
      words[3] = "Introduce temperatura";
    }
  }
  else {
    words[0]  = "Temperature";
    words[1] = "LEDS";
    words[2] = "Settings";
    if (enableSet) {
      words[3] = "Start";
    }
    else {
      words[3] = "Set new Temperature";
    }
  }
  rectangles = 4;
  drawRectangles();
  drawHeading();
  updateSensors();
  while (!digitalRead(pulse)) {
    updateData();
  }
  delay(100);
  selectMode();
}

