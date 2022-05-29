
void UI_mainMenu() {
  page = mainMenuPage;
  byte numWords = 5;
  print_text = true;
  tft.setTextSize(1);
  tft.setTextColor(COLOR_MENU_TEXT);
  for (int i = false; i < numWords; i++) {
    pos_text[i] = LEFT_MARGIN;
  }
  pos_text[startGraphicPosition] = CENTER;
  switch (language) {
    case spanish:
      words[controlModeGraphicPosition]  = convertStringToChar("Modo de control");
      if (controlMode) {
        words[temperatureGraphicPosition]  = convertStringToChar("Temp aire");
      }
      else {
        words[temperatureGraphicPosition]  = convertStringToChar("Temp piel");
      }
      words[humidityGraphicPosition] = convertStringToChar("Humedad");
      words[LEDGraphicPosition] = convertStringToChar("Fototerapia");
      break;
    case english:
      words[controlModeGraphicPosition]  = convertStringToChar("Control mode");
      if (controlMode) {
        words[temperatureGraphicPosition]  = convertStringToChar("Air temp");
      }
      else {
        words[temperatureGraphicPosition]  = convertStringToChar("Skin temp");
      }
      words[humidityGraphicPosition] = convertStringToChar("Humidity");
      words[LEDGraphicPosition] = convertStringToChar("Phototherapy");
      break;
    case french:
      words[controlModeGraphicPosition]  = convertStringToChar("Mode de controle");
      if (controlMode) {
        words[temperatureGraphicPosition]  = convertStringToChar("Temp air");
      }
      else {
        words[temperatureGraphicPosition]  = convertStringToChar("Temp peau");
      }
      words[humidityGraphicPosition] = convertStringToChar("Humidite");
      words[LEDGraphicPosition] = convertStringToChar("Phototherapie");
      break;
    case portuguese:
      words[controlModeGraphicPosition]  = convertStringToChar("Modo de controle");
      if (controlMode) {
        words[temperatureGraphicPosition]  = convertStringToChar("Temp do ar");
      }
      else {
        words[temperatureGraphicPosition]  = convertStringToChar("Temp pele");
      }
      words[humidityGraphicPosition] = convertStringToChar("Umidade");
      words[LEDGraphicPosition] = convertStringToChar("Fototerapia");
      break;
  }
  words[startGraphicPosition] = convertStringToChar("");
  rectangles = numWords;
  setSensorsGraphicPosition();
  goToProcessRow = numWords;
  graphics();
  drawHeading();
  updateDisplaySensors();
  controlTemperature = false;
  controlHumidity = false;
  enableSet = false;
  bar_pos = true;
  selected = false;
  ypos = graphicHeight(bar_pos - 1);
  if (desiredControlTemperature < minDesiredTemp[controlMode] || desiredControlTemperature > maxDesiredTemp[controlMode]) {
    desiredControlTemperature = presetTemp[controlMode];
  }
  while (!GPIORead(ENC_SWITCH)) {
    updateData();
  }
  vTaskDelay(debounceTime);
}
