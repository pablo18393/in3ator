void firstTurnOn() {
  analogWrite(SCREENBACKLIGHT, backlight_intensity);
  byte numWords = 2;
  tft.setTextSize(1);
  tft.fillScreen(introBackColor);
  tft.setTextColor(introTextColor);
  for (int i = 0; i < numWords; i++) {
    pos_text[i] = 0;
  }
  if (!language) {
    words[0]  = "pulse to start";
    words[1]  = "Welcome to in3";
  }
  else {
    words[0]  = "pulsa para empezar";
    words[1]  = "Bienvenido a in3";
  }
  for (int i = 0; i < numWords; i++) {
    tft.drawCentreString(words[i], tft.width() / 2, tft.height() / 2 - letter_height * 2 * i , textFontSize);
  }
  while (digitalRead(pulse));
  delay(100);
}

