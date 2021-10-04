
void advancedMode() {
  page = advancedModePage;
  byte numWords = 4;
  print_text = true;
  tft.setTextSize(1);
  tft.setTextColor(COLOR_MENU_TEXT);
  for (int i = false; i < numWords; i++) {
    pos_text[i] = leftMargin;
  }
  pos_text[startGraphicPosition] = centered;
  switch (language) {
    case spanish:
      words[temperatureGraphicPosition]  = "Temperatura";
      words[humidityGraphicPosition] = "Humedad";
      words[LEDGraphicPosition] = "Fototerapia";
      break;
    case english:
      words[temperatureGraphicPosition]  = "Temperature";
      words[humidityGraphicPosition] = "Humidity";
      words[LEDGraphicPosition] = "Phototherapy";
      break;
    case french:
      words[temperatureGraphicPosition]  = "Temperature";
      words[humidityGraphicPosition] = "Humidite";
      words[LEDGraphicPosition] = "Phototherapie";
      break;
    case portuguese:
      words[temperatureGraphicPosition]  = "Temperatura";
      words[humidityGraphicPosition] = "Umidade";
      words[LEDGraphicPosition] = "Fototerapia";
      break;
  }
  words[startGraphicPosition] = "";
  rectangles = numWords;
  setSensorsGraphicPosition();
  goToProcessRow = numWords;
  graphics();
  drawHeading();
  updateDisplaySensors();
  controlTemperature = false;
  controlHumidity = false;
  enableSet = false;
  while (!GPIORead(ENC_SWITCH)) {
    updateData();
  }
  delay(debounceTime);
  barSelection();
}
