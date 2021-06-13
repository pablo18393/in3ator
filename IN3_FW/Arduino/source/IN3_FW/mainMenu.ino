
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
      words[LEDGraphicPosition] = "Fototerapia";
      break;
    case english:
      words[gestationGraphicPosition]  = "Gestation weeks";
      words[advancedModeGraphicPosition] = "Advanced mode";
      words[LEDGraphicPosition] = "Phototherapy";
      break;
    case french:
      words[gestationGraphicPosition]  = "Semaines gestation";
      words[advancedModeGraphicPosition] = "Mode avance";
      words[LEDGraphicPosition] = "Phototherapie";
      break;
    case portuguese:
      words[gestationGraphicPosition]  = "Semanas gestacao";
      words[advancedModeGraphicPosition] = "Modo avancado";
      words[LEDGraphicPosition] = "Fototerapia";
      break;
  }
  words[startGraphicPosition] = "";
  rectangles = numWords;
  setSensorsGraphicPosition();
  goToProcessRow = numWords;
  graphics();
  drawHeading();
  controlTemperature = 0;
  controlHumidity = 0;
  enableSet = 0;
  while (!digitalRead(ENC_SWITCH)) {
    updateData();
  }
  delay(debounceTime);
  updateData();
  barSelection();
}

void askSuccess() {
  if (page == actuatorsProgressPage) {
    stopHeaterPID();
    turnActuators(OFF);
    turnFans(OFF);
    GPRSSetPostVariables(actuatorsModeOFF, "");
    setGPRSPostPeriod(standByGPRSPostPeriod);
  }
  page = askSuccessPage;
  bar_pos = 2;
  byte numWords = 3;
  print_text = 1;
  tft.setTextSize(1);
  tft.setTextColor(COLOR_MENU_TEXT);
  for (int i = 0; i < numWords; i++) {
    pos_text[i] = leftMargin;
  }
  pos_text[afirmativeGraphicPosition] = centered;
  pos_text[negativeGraphicPosition] = centered;
  switch (language) {
    case english:
      words[successQuestionGraphicPosition] = "IS THE BABY ALIVE?";
      words[afirmativeGraphicPosition] = "YES";
      words[negativeGraphicPosition] = "NO";
      break;
    case spanish:
      words[successQuestionGraphicPosition] = "¿HA SOBREVIVIDO EL BEBE?";
      words[afirmativeGraphicPosition] = "SI";
      words[negativeGraphicPosition] = "NO";
      break;
    case french:
      words[successQuestionGraphicPosition] = "LE BEBE ATIL SURVECU?";
      words[afirmativeGraphicPosition] = "OUI";
      words[negativeGraphicPosition] = "PAS";
      break;
    case portuguese:
      words[successQuestionGraphicPosition] = "O BEBE SOBREVIVIU?";
      words[afirmativeGraphicPosition] = "SIM";
      words[negativeGraphicPosition] = "NAO";
      break;
  }

  rectangles = numWords;
  setSensorsGraphicPosition();
  goToProcessRow = numWords;
  graphics();
  drawHeading();
  while (!digitalRead(ENC_SWITCH)) {
    updateData();
  }
  delay(debounceTime);
  barSelection();
}

bool checkAskSuccessPage() {
  if (page == askSuccessPage) {
    return true;
  }
  return false;
}
