void menu() {
  page = 0;
  setVariablesPosition();
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_WHITE);
  for (int i = 0; i <= 3; i++) {
    pos_text[i] = 0;
  }
  pos_text[3] = 1;
  if (language) {
    words[0]  = "Temperatura";
    words[1] = "LEDS";
    words[2] = "Lenguaje";
    if (enableSet) {
      words[3] = "Empezar";
    }
  }
  else {
    words[0]  = "Temperature";
    words[1] = "LEDS";
    words[2] = "Language";
    if (enableSet) {
      words[3] = "Set";
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

