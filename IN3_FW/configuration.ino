void configuration() {
  page = 2;
  tft.setTextSize(1);
  tft.setTextColor(COLOR_MENU_TEXT);
  for (int i = 0; i <= 3; i++) {
    pos_text[i] = 0;
  }
  pos_text[2] = 1;
  if (!language) {
    words[0]  = "AUTO LOCK";
    words[1] = "LANGUAGE";
    words[2] = "CALIBRATE";
  }
  else {
    words[0]  = "AUTO BLOQUEO";
    words[1] = "IDIOMA";
    words[2] = "CALIBRAR";
  }
  rectangles = 3;
  drawRectangles();
  drawHeading();
  while (!digitalRead(pulse)) {
    updateData();
  }
  delay(100);
  selectMode();
}

