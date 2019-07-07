void mainMenu() {
  page = mainMenuPage;
  byte numWords = 4;
  print_text = 1;
  tft.setTextSize(1);
  tft.setTextColor(COLOR_MENU_TEXT);
  for (int i = 0; i < numWords; i++) {
    pos_text[i] = leftMargin;
  }
  pos_text[advancedModeGraphicPosition] = centered;
  pos_text[startGraphicPosition] = centered;
  switch (language) {
    case spanish:
      words[gestationGraphicPosition]  = "Semanas gestacion";
      words[advancedModeGraphicPosition] = "Modo avanzado";
      break;
    case english:
      words[gestationGraphicPosition]  = "Gestation weeks";
      words[advancedModeGraphicPosition] = "Advanced mode";
      break;
    case french:
      words[gestationGraphicPosition]  = "Semaines de gestation";
      words[advancedModeGraphicPosition] = "Mode avance";
      break;
  }
  words[LEDGraphicPosition] = "LED";
  words[startGraphicPosition] = "";
  rectangles = numWords;
  setSensorsGraphicPosition();
  goToProcessRow = numWords;
  graphics();
  drawHeading();
  updateSensors();
  controlTemperature = 0;
  controlHumidity = 0;
  enableSet = 0;
  while (!digitalRead(ENC_PULSE)) {
    updateData();
  }
  delay(debounceTime);
  barSelection();
}

void askSuccess() {
  if (page == actuatorsProgressPage) {
    if (temperaturePIDcontrol) {
      stopPID();
    }
    turnActuatorsOff();
    turnFansOff();
  }
  page = askSuccessPage;
  byte numWords = 3;
  print_text = 1;
  tft.setTextSize(1);
  tft.setTextColor(COLOR_MENU_TEXT);
  for (int i = 0; i < numWords; i++) {
    pos_text[i] = leftMargin;
  }
  words[successQuestionGraphicPosition] = "IS THE BABY ALIVE?";
  words[afirmativeGraphicPosition] = "YES";
  words[negativeGraphicPosition] = "NO";
  rectangles = numWords;
  setSensorsGraphicPosition();
  goToProcessRow = numWords;
  graphics();
  drawHeading();
  while (!digitalRead(ENC_PULSE)) {
    updateData();
  }
  delay(debounceTime);
  barSelection();
}
