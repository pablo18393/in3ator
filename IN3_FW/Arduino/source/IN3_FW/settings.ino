
void settings() {
  byte numWords = 7;
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
      words[controlModeGraphicPosition] = "Control mode";
      words[heaterPowerGraphicPosition] = "Heater power";
      words[DebugENGraphicPosition] = "UART debug";
      words[setStandardValuesGraphicPosition] = "Set standard values";
      words[calibrateGraphicPosition] = "Calibrate";
      break;
    case spanish:
      words[autoLockGraphicPosition]  = "Auto bloqueo";
      words[languageGraphicPosition] = "Idioma";
      words[controlModeGraphicPosition] = "Modo de control";
      words[heaterPowerGraphicPosition] = "Pot calefactor";
      words[DebugENGraphicPosition] = "UART debug";
      words[setStandardValuesGraphicPosition] = "Fijar valores estandar";
      words[calibrateGraphicPosition] = "Calibrar";
      break;
    case french:
      words[autoLockGraphicPosition]  = "Verrouillage auto";
      words[languageGraphicPosition] = "Langue";
      words[controlModeGraphicPosition] = "Mode de controle";
      words[heaterPowerGraphicPosition] = "Pot chauffage";
      words[DebugENGraphicPosition] = "UART debug";
      words[setStandardValuesGraphicPosition] = "Definir valeurs standard";
      words[calibrateGraphicPosition] = "Etalonner";
      break;
    case portuguese:
      words[autoLockGraphicPosition]  = "Bloqueio automatico";
      words[languageGraphicPosition] = "Idioma";
      words[controlModeGraphicPosition] = "Modo de controle";
      words[heaterPowerGraphicPosition] = "Pot de aquecimento";
      words[DebugENGraphicPosition] = "UART debug";
      words[setStandardValuesGraphicPosition] = "Definir valores de fabrica";
      words[calibrateGraphicPosition] = "Calibrar";
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
