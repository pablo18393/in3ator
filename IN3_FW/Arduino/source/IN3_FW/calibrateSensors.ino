
void calibrateSensors() {
  byte numWords = 3;
  page = calibrateSensorsPage;
  print_text = true;
  tft.setTextSize(1);
  tft.setTextColor(COLOR_MENU_TEXT);
  for (int i = false; i < numWords; i++) {
    pos_text[i] = leftMargin;
  }
  pos_text[2] = true;
  switch (language) {
    case english:
      words[temperatureCalibrationGraphicPosition]  = "Temperature";
      words[humidityCalibrationGraphicPosition] = "Humidity";
      words[restartCalibrationValuesTempGraphicPosition] = "Reset values";
      break;
    case spanish:
      words[temperatureCalibrationGraphicPosition]  = "Temperatura";
      words[humidityCalibrationGraphicPosition] = "Humedad";
      words[restartCalibrationValuesTempGraphicPosition] = "Reiniciar valores";
      break;
    case french:
      words[temperatureCalibrationGraphicPosition]  = "Temperature";
      words[humidityCalibrationGraphicPosition] = "Humidite";
      words[restartCalibrationValuesTempGraphicPosition] = "Reinitialiser valeurs";
      break;
    case portuguese:
      words[temperatureCalibrationGraphicPosition]  = "Temperatura";
      words[humidityCalibrationGraphicPosition] = "Umidade";
      words[restartCalibrationValuesTempGraphicPosition] = "Redefinir valores";
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
