
void mainMenu() {
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
      words[controlModeGraphicPosition]  = "Modo de control";
      if (controlMode) {
        words[temperatureGraphicPosition]  = "Temp aire";
      }
      else {
        words[temperatureGraphicPosition]  = "Temp piel";
      }
      words[humidityGraphicPosition] = "Humedad";
      words[LEDGraphicPosition] = "Fototerapia";
      break;
    case english:
      words[controlModeGraphicPosition]  = "Control mode";
      if (controlMode) {
        words[temperatureGraphicPosition]  = "Air temp";
      }
      else {
        words[temperatureGraphicPosition]  = "Skin temp";
      }
      words[humidityGraphicPosition] = "Humidity";
      words[LEDGraphicPosition] = "Phototherapy";
      break;
    case french:
      words[controlModeGraphicPosition]  = "Mode de controle";
      if (controlMode) {
        words[temperatureGraphicPosition]  = "Temp air";
      }
      else {
        words[temperatureGraphicPosition]  = "Temp peau";
      }
      words[humidityGraphicPosition] = "Humidite";
      words[LEDGraphicPosition] = "Phototherapie";
      break;
    case portuguese:
      words[controlModeGraphicPosition]  = "Modo de controle";
      if (controlMode) {
        words[temperatureGraphicPosition]  = "Temp do ar";
      }
      else {
        words[temperatureGraphicPosition]  = "Temp pele";
      }
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
  bar_pos = true;
  selected = false;
  ypos = graphicHeight(bar_pos - 1);
  desiredRoomHum = presetHumidity;
  if (desiredControlTemp < minDesiredTemp[controlMode] || desiredControlTemp > maxDesiredTemp[controlMode]) {
    desiredControlTemp = presetTemp[controlMode];
  }
  while (!GPIORead(ENC_SWITCH)) {
    updateData();
  }
  delay(debounceTime);
}
