void menu() {
  if (page == actuatorsProgressPage) {
    if (PIDcontrol) {
      stopPID();
    }
    turnFansOff();
  }
  page = menuPage;
  byte numWords = 5;
  print_text = 1;
  tft.setTextSize(1);
  tft.setTextColor(COLOR_MENU_TEXT);
  for (int i = 0; i < numWords; i++) {
    pos_text[i] = 0;
  }
  pos_text[settingsGraphicPosition] = 1;
  pos_text[startGraphicPosition] = 1;
  switch (language) {
    case spanish:
      words[temperatureGraphicPosition]  = "Temperatura";
      words[humidityGraphicPosition] = "Humedad";
      words[LEDGraphicPosition] = "LED";
      words[settingsGraphicPosition] = "Configuracion";
      if (enableSet) {
        words[startGraphicPosition] = "Empezar";
      }
      else {
        words[startGraphicPosition] = "";
      }
      break;
    case english:
      words[temperatureGraphicPosition]  = "Temperature";
      words[humidityGraphicPosition] = "Humedad";
      words[LEDGraphicPosition] = "LED";
      words[settingsGraphicPosition] = "Settings";
      if (enableSet) {
        words[startGraphicPosition] = "Start";
      }
      else {
        words[startGraphicPosition] = "";
      }
      break;
  }
  rectangles = numWords;
  setVariablesPosition();
  goToProcessRow = numWords;
  drawGraphicInterface();
  drawHeading();
  updateSensors();
  while (!digitalRead(pulse)) {
    updateData();
  }
  delay(debounceTime);
  barSelection();
}

