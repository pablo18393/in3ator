void firstTurnOn() {
  byte numWords = 1;
  page = 2;
  tft.setTextSize(1);
  tft.setTextColor(COLOR_MENU_TEXT);
  for (int i = 0; i < numWords; i++) {
    pos_text[i] = 0;
  }
  if (!language) {
    words[0]  = "Welcome to in3";
  }
  else {
    words[0]  = "Bienvenido a in3";
  }
  rectangles = numWords;
  drawGraphicInterface();
  drawHeading();
  while (digitalRead(pulse)) {
    updateData();
  }
  delay(100);
}

