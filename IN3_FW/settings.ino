
void settings() {
  byte numWords = 6;
  page = settingsPage;
  print_text = 1;
  tft.setTextSize(1);
  tft.setTextColor(COLOR_MENU_TEXT);
  for (int i = 0; i < numWords; i++) {
    pos_text[i] = leftMargin;
  }
  pos_text[setStandardValuesGraphicPosition] = centered;
  pos_text[calibrateGraphicPosition] = centered;
  switch (language) {
    case english:
      words[autoLockGraphicPosition]  = "Auto lock";
      words[languageGraphicPosition] = "Language";
      words[heaterTempGraphicPosition] = "Heater temp";
      words[fanGraphicPosition] = "Fan speed";
      words[setStandardValuesGraphicPosition] = "Set standard values";
      words[calibrateGraphicPosition] = "Calibrate";
      break;
    case spanish:
      words[autoLockGraphicPosition]  = "Auto bloqueo";
      words[languageGraphicPosition] = "Idioma";
      words[heaterTempGraphicPosition] = "Temp calefactor";
      words[fanGraphicPosition] = "Ventilador";
      words[setStandardValuesGraphicPosition] = "Fijar valores estandar";
      words[calibrateGraphicPosition] = "Calibrar";
      break;
    case french:
      words[autoLockGraphicPosition]  = "verrouillage auto";
      words[languageGraphicPosition] = "Langue";
      words[heaterTempGraphicPosition] = "Temp chauffage";
      words[fanGraphicPosition] = "Ventilateur";
      words[setStandardValuesGraphicPosition] = "definir valeurs standard";
      words[calibrateGraphicPosition] = "etalonner";
      break;
  }
  rectangles = numWords;
  graphics();
  drawHeading();
  while (!digitalRead(ENC_SWITCH)) {
    updateData();
  }
  delay(debounceTime);
  barSelection();
}
