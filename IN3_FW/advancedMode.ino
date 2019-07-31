void advancedMode() {
  page = advancedModePage;
  byte numWords = 4;
  print_text = 1;
  tft.setTextSize(1);
  tft.setTextColor(COLOR_MENU_TEXT);
  for (int i = 0; i < numWords; i++) {
    pos_text[i] = leftMargin;
  }
  pos_text[startGraphicPosition] = centered;
  switch (language) {
    case spanish:
      words[temperatureGraphicPosition]  = "Temperatura";
      words[humidityGraphicPosition] = "Humedad";
      break;
    case english:
      words[temperatureGraphicPosition]  = "Temperature";
      words[humidityGraphicPosition] = "Humedad";
      break;
    case french:
      words[temperatureGraphicPosition]  = "Temperature";
      words[humidityGraphicPosition] = "Humidite";
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
  while (!digitalRead(ENC_SWITCH)) {
    updateData();
  }
  delay(debounceTime);
  barSelection();
}
