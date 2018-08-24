void firstTurnOn() {
  analogWrite(SCREENBACKLIGHT, backlight_intensity);
  byte numWords = 2;
  print_text = 1;
  tft.setTextSize(1);
  tft.setTextColor(introBackColor);
  drawIntroMessage();
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
  delay(debounceTime);
  void helpingMenu();
  displayMenuHelp();
  displayTemperatureHelp();
  displaySettingsHelp();
  displayStartHelp();
  while (1);
}

void helpingMenu() {
  tft.fillScreen(COLOR_MENU);
  page = menuPage;
  byte  numWords = 4;
  setSensorsGraphicPosition();
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
      words[3] = "";
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
      words[3] = "";
    }
  }
  rectangles = numWords;
  goToProcessRow = numWords;
  drawGraphicInterface();
  drawHeading();
  updateSensors();
  while (!digitalRead(pulse));
  delay(debounceTime);
}

void waitForPulse() {
  while (digitalRead(pulse));
  delay(debounceTime);
  helpingMenu();
  while (!digitalRead(pulse));
  delay(debounceTime);
}

void displayMenuHelp() {
  drawMenuHelpText();
  waitForPulse();
}

void displayTemperatureHelp() {
  waitForPulse();
}

void displaySettingsHelp() {
  waitForPulse();
}
void displayStartHelp() {
  waitForPulse();
}

void drawMenuHelpText() {
}
