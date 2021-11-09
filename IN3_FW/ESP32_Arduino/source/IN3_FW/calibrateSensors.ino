
void calibrateSensors() {
  byte numWords = 5;
  page = calibrateSensorsPage;
  print_text = true;
  tft.setTextSize(1);
  setTextColor(COLOR_MENU_TEXT);
  for (int i = false; i < numWords; i++) {
    pos_text[i] = leftMargin;
  }
  pos_text[3] = true;
  pos_text[4] = true;
  switch (language) {
    case english:
      words[airTemperatureCalibrationGraphicPosition]  = "Air temperature";
      words[skinTemperatureCalibrationGraphicPosition]  = "Skin temperature";
      words[humidityCalibrationGraphicPosition] = "Humidity";
      words[autoCalibrationGraphicPosition] = "Auto calibration";
      words[restartCalibrationGraphicPosition] = "Reset values";
      break;
    case spanish:
      words[airTemperatureCalibrationGraphicPosition]  = "Temperaturea aire";
      words[skinTemperatureCalibrationGraphicPosition]  = "Temperaturea piel";
      words[humidityCalibrationGraphicPosition] = "Humedad";
      words[autoCalibrationGraphicPosition] = "Auto calibracion";
      words[restartCalibrationGraphicPosition] = "Reiniciar valores";
      break;
    case french:
      words[airTemperatureCalibrationGraphicPosition]  = "Temperature air";
      words[skinTemperatureCalibrationGraphicPosition]  = "Temperature peau";
      words[humidityCalibrationGraphicPosition] = "Humidite";
      words[autoCalibrationGraphicPosition] = "Auto-etalonnage";
      words[restartCalibrationGraphicPosition] = "Reinitialiser valeurs";
      break;
    case portuguese:
      words[airTemperatureCalibrationGraphicPosition]  = "Temperature do ar";
      words[skinTemperatureCalibrationGraphicPosition]  = "Temperaturaa pele";
      words[humidityCalibrationGraphicPosition] = "Umidade";
      words[autoCalibrationGraphicPosition] = "Auto calibracao";
      words[restartCalibrationGraphicPosition] = "Redefinir valores";
      break;
  }
  rectangles = numWords;
  graphics();
  drawHeading();
  while (!GPIORead(ENC_SWITCH)) {
    updateData();
  }
  delay(debounceTime);
  barSelection();
}
