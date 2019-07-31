void firstTurnOn() {
  analogWrite(SCREENBACKLIGHT, backlight_intensity);
  byte numWords = 2;
  print_text = 1;
  tft.setTextSize(1);
  tft.setTextColor(introBackColor);
  drawIntroMessage();
  tft.setTextColor(introTextColor);
  for (int i = 0; i < numWords; i++) {
    pos_text[i] = leftMargin;
  }
  if (!language) {
    words[0]  = "ENC_SWITCH to start";
    words[1]  = "Welcome to in3";
  }
  else {
    words[0]  = "pulsa para empezar";
    words[1]  = "Bienvenido a in3";
  }
  for (int i = 0; i < numWords; i++) {
    tft.drawCentreString(words[i], tft.width() / 2, tft.height() / 2 - letter_height * 2 * i , textFontSize);
  }
  while (digitalRead(ENC_SWITCH));
  delay(debounceTime);
  void helpingmainMenu();
  displayMenuHelp();
  displayTemperatureHelp();
  displaySettingsHelp();
  displayStartHelp();
  while (1);
}

void helpingmainMenu() {
  tft.fillScreen(COLOR_MENU);
  page = advancedModePage;
  byte  numWords = 4;
  setSensorsGraphicPosition();
  tft.setTextSize(1);
  tft.setTextColor(COLOR_MENU_TEXT);
  for (int i = 0; i < numWords; i++) {
    pos_text[i] = leftMargin;
  }
  pos_text[2] = centered;
  pos_text[3] = centered;
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
  graphics();
  drawHeading();
  updateSensors();
  while (!digitalRead(ENC_SWITCH));
  delay(debounceTime);
}

void waitForENC_SWITCH() {
  while (digitalRead(ENC_SWITCH));
  delay(debounceTime);
  helpingmainMenu();
  while (!digitalRead(ENC_SWITCH));
  delay(debounceTime);
}

void displayMenuHelp() {
  drawMenuHelpText();
  waitForENC_SWITCH();
}

void displayTemperatureHelp() {
  waitForENC_SWITCH();
}

void displaySettingsHelp() {
  waitForENC_SWITCH();
}
void displayStartHelp() {
  waitForENC_SWITCH();
}

void drawMenuHelpText() {
}
