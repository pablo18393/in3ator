void menu() {
  tft.setTextSize(1);
  tft.setTextColor(ILI9341_WHITE);
  for (int i = 0; i <= 3; i++) {
    pos_text[i] = 0;
  }
  if (language) {
    words[0]  = "TEMPERATURA";
    words[1] = "HUMEDAD";
    words[2] = "LEDS";
    words[3] = "LENGUAJE";
  }
  else {
    words[0]  = "TEMPERATURE";
    words[1] = "HUMIDITY";
    words[2] = "LEDS";
    words[3] = "LANGUAGE";
  }
  rectangles = 4;
  manual_speed = 0;
  drawRectangles();
  drawHeading();
  while (digitalRead(pulse) == 0){
    updateData();
  }
  delay(100);
  selectMode();
}

