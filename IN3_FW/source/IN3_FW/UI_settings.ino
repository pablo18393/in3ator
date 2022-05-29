void UI_settings() {
  byte numWords = 6;
  page = settingsPage;
  print_text = true;
  tft.setTextSize(1);
  setTextColor(COLOR_MENU_TEXT);
  for (int i = false; i < numWords; i++) {
    pos_text[i] = LEFT_MARGIN;
  } 
  pos_text[setdefaultValuesGraphicPosition] = CENTER;
  pos_text[HWTestGraphicPosition] = CENTER;
  pos_text[calibrateGraphicPosition] = CENTER;
  switch (language) {
    case english:
      words[autoLockGraphicPosition]  = convertStringToChar("Auto lock");
      words[languageGraphicPosition] = convertStringToChar("Language");
      words[controlAlgorithmGraphicPosition] = convertStringToChar("Control algorithm");
      words[serialNumberGraphicPosition] = convertStringToChar("Serial number");
      words[setdefaultValuesGraphicPosition] = convertStringToChar("Set default values");
      words[calibrateGraphicPosition] = convertStringToChar("Calibration");
      break;
    case spanish:
      words[autoLockGraphicPosition]  = convertStringToChar("Auto bloqueo");
      words[languageGraphicPosition] = convertStringToChar("Idioma");
      words[controlAlgorithmGraphicPosition] = convertStringToChar("Algoritmo control");
      words[serialNumberGraphicPosition] = convertStringToChar("Serial number");
      words[setdefaultValuesGraphicPosition] = convertStringToChar("Fijar valores estandar");
      words[calibrateGraphicPosition] = convertStringToChar("Calibracion");
      break;
    case french:
      words[autoLockGraphicPosition]  = convertStringToChar("Verrouillage auto");
      words[languageGraphicPosition] = convertStringToChar("Langue");
      words[controlAlgorithmGraphicPosition] = convertStringToChar("Controle algorithme");
      words[serialNumberGraphicPosition] = convertStringToChar("Serial number");
      words[setdefaultValuesGraphicPosition] = convertStringToChar("Definir valeurs default");
      words[calibrateGraphicPosition] = convertStringToChar("Etalonner");
      break;
    case portuguese:
      words[autoLockGraphicPosition]  = convertStringToChar("Bloqueio automatico");
      words[languageGraphicPosition] = convertStringToChar("Idioma");
      words[controlAlgorithmGraphicPosition] = convertStringToChar("Algoritmo controle");
      words[serialNumberGraphicPosition] = convertStringToChar("Serial number");
      words[setdefaultValuesGraphicPosition] = convertStringToChar("Definir valores de fabrica");
      words[calibrateGraphicPosition] = convertStringToChar("Calibrar");
      break;
  }
  words[WifiENGraphicPosition] = convertStringToChar("WIFI");
  words[HWTestGraphicPosition] = convertStringToChar("HW Test");
  if (WiFi.status() == WL_CONNECTED ) {
    words[WifiENGraphicPosition] = convertStringToChar("WIFI IP->");
  }
  rectangles = numWords;
  graphics();
  drawHeading();
  bar_pos = true;
  selected = false;
  ypos = graphicHeight(bar_pos - 1);
  while (!GPIORead(ENC_SWITCH)) {
    updateData();
  }
  vTaskDelay(debounceTime);
}
