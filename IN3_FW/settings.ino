void settings() {
  page = 2;
  EEPROM.write(0, page);
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_WHITE);
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

