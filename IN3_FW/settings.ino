void settings() {
  byte numWords = 5;
  page = settingsPage;
  print_text = 1;
  tft.setTextSize(1);
  tft.setTextColor(COLOR_MENU_TEXT);
  for (int i = 0; i < numWords; i++) {
    pos_text[i] = 0;
  }
  pos_text[setStandardValuesGraphicPosition] = 1;
  pos_text[calibrateGraphicPosition] = 1;
  switch (language) {
    case english:
      words[autoLockGraphicPosition]  = "Auto lock";
      words[languageGraphicPosition] = "Language";
      words[heaterTempGraphicPosition] = "Heater temp";
      words[setStandardValuesGraphicPosition] = "Set standard values";
      words[calibrateGraphicPosition] = "Calibrate";
      break;
    case spanish:
      words[autoLockGraphicPosition]  = "Auto bloqueo";
      words[languageGraphicPosition] = "Idioma";
      words[heaterTempGraphicPosition] = "Temp calefactor";
      words[setStandardValuesGraphicPosition] = "Fijar valores iniciales";
      words[calibrateGraphicPosition] = "Calibrar";
      break;
  }
  rectangles = numWords;
  drawGraphicInterface();
  drawHeading();
  updateSensors();
  while (!digitalRead(pulse)) {
    updateData();
  }
  delay(debounceTime);
  barSelection();
}



