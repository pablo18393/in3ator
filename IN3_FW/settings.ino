void settings() {
  page = 2;
  tft.setTextSize(1);
  tft.setTextColor(COLOR_MENU_TEXT);
  for (int i = 0; i <= 0; i++) {
    pos_text[i] = 0;
  }
  if (language) {
    words[0]  = "IDIOMA";
  }
  else {
    words[0]  = "LANGUAGE";
  }
  rectangles = 1;
  drawHeading();
  drawRectangles();
  while (digitalRead(pulse) == 0);
  delay(50);
  selectMode();
}

