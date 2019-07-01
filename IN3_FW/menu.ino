void menu() {
  page = menuPage;
  byte numWords = 4;
  print_text = 1;
  tft.setTextSize(1);
  tft.setTextColor(COLOR_MENU_TEXT);
  for (int i = 0; i < numWords; i++) {
    pos_text[i] = leftMargin;
  }
  pos_text[settingsGraphicPosition] = centered;
  pos_text[startGraphicPosition] = centered;

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
  while (!digitalRead(pulse)) {
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
  while (!digitalRead(pulse)) {
    updateData();
  }
  delay(debounceTime);
  barSelection();
}
