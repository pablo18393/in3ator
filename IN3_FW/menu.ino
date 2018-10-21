void menu() {
  if (page == actuatorsProgressPage) {
    if (temperaturePIDcontrol) {
      stopPID();
    }
    turnActuatorsOff();
    turnFansOff();
  }
  page = menuPage;
  byte numWords = 4;
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
      words[settingsGraphicPosition] = "Configuracion";
      break;
    case english:
      words[temperatureGraphicPosition]  = "Temperature";
      words[humidityGraphicPosition] = "Humedad";
      words[settingsGraphicPosition] = "Settings";
      break;
    case french:
      words[temperatureGraphicPosition]  = "Temperature";
      words[humidityGraphicPosition] = "Humidite";
      words[settingsGraphicPosition] = "parametres";
      break;
  }
  words[LEDGraphicPosition] = "LED";
  words[startGraphicPosition] = "";
  rectangles = numWords;
  setSensorsGraphicPosition();
  goToProcessRow = numWords;
  drawGraphicInterface();
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

