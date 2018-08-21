void calibrateSensors() {
  byte numWords = 3;
  page = calibrateSensorsPage;
  print_text = 1;
  tft.setTextSize(1);
  tft.setTextColor(COLOR_MENU_TEXT);
  for (int i = 0; i < numWords; i++) {
    pos_text[i] = 0;
  }
  pos_text[2] = 1;
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
  }
  rectangles = numWords;
  drawGraphicInterface();
  drawHeading();
  updateSensors();
  while (!digitalRead(pulse)) {
    updateData();
  }
  delay(debounceTime);
  barSelection();
}
