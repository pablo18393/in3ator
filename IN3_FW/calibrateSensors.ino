void calibrateSensors() {
  byte numWords = 3;
  page = 3;
  tft.setTextSize(1);
  tft.setTextColor(COLOR_MENU_TEXT);
  for (int i = 0; i < numWords; i++) {
    pos_text[i] = 0;
  }
  pos_text[2] = 1;
  if (!language) {
    words[0]  = "Temperature";
    words[1] = "Humidity";
    words[2] = "Reset values";
  }
  else {
    words[0]  = "Temperatura";
    words[1] = "Humedad";
    words[2] = "Reiniciar valores";
  }
  rectangles = 3;
  drawRectangles();
  drawHeading();
  updateSensors();
  while (!digitalRead(pulse)) {
    updateData();
  }
  delay(100);
  selectMode();
}
