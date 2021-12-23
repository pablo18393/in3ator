void settings() {
  byte numWords = 7;
  page = settingsPage;
  print_text = true;
  tft.setTextSize(1);
  setTextColor(COLOR_MENU_TEXT);
  for (int i = false; i < numWords; i++) {
    pos_text[i] = LEFT_MARGIN;
  }
  pos_text[setdefaultValuesGraphicPosition] = CENTER;
  pos_text[calibrateGraphicPosition] = CENTER;
  switch (language) {
    case english:
      words[autoLockGraphicPosition]  = "Auto lock";
      words[languageGraphicPosition] = "Language";
      words[controlAlgorithmGraphicPosition] = "Control algorithm";
      words[heaterPowerGraphicPosition] = "Heater power";
      words[WifiENGraphicPosition] = "WIFI";
      words[setdefaultValuesGraphicPosition] = "Set default values";
      words[calibrateGraphicPosition] = "Calibration";
      break;
    case spanish:
      words[autoLockGraphicPosition]  = "Auto bloqueo";
      words[languageGraphicPosition] = "Idioma";
      words[controlAlgorithmGraphicPosition] = "Algoritmo control";
      words[heaterPowerGraphicPosition] = "Pot calefactor";
      words[WifiENGraphicPosition] = "WIFI";
      words[setdefaultValuesGraphicPosition] = "Fijar valores estandar";
      words[calibrateGraphicPosition] = "Calibracion";
      break;
    case french:
      words[autoLockGraphicPosition]  = "Verrouillage auto";
      words[languageGraphicPosition] = "Langue";
      words[controlAlgorithmGraphicPosition] = "Controle algorithme";
      words[heaterPowerGraphicPosition] = "Pot chauffage";
      words[WifiENGraphicPosition] = "WIFI";
      words[setdefaultValuesGraphicPosition] = "Definir valeurs default";
      words[calibrateGraphicPosition] = "Etalonner";
      break;
    case portuguese:
      words[autoLockGraphicPosition]  = "Bloqueio automatico";
      words[languageGraphicPosition] = "Idioma";
      words[controlAlgorithmGraphicPosition] = "Algoritmo controle";
      words[heaterPowerGraphicPosition] = "Pot de aquecimento";
      words[WifiENGraphicPosition] = "WIFI";
      words[setdefaultValuesGraphicPosition] = "Definir valores de fabrica";
      words[calibrateGraphicPosition] = "Calibrar";
      break;
  }
  rectangles = numWords;
  graphics();
  drawHeading();
  bar_pos = true;
  ypos = graphicHeight(bar_pos - 1);
  while (!GPIORead(ENC_SWITCH)) {
    updateData();
  }
  delay(debounceTime);
}
