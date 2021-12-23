
void calibrateSensors() {
  byte numWords = 2;
  page = calibrateSensorsPage;
  print_text = true;
  tft.setTextSize(1);
  setTextColor(COLOR_MENU_TEXT);
  for (int i = false; i < numWords; i++) {
    pos_text[i] = CENTER;
  }
  switch (language) {
    case english:
      words[twoPointCalibrationGraphicPosition]  = "2-p calibration";
      words[restartCalibrationGraphicPosition] = "Reset values";
      break;
    case spanish:
      words[twoPointCalibrationGraphicPosition]  = "2-p calibration";
      words[restartCalibrationGraphicPosition] = "Reiniciar valores";
      break;
    case french:
      words[twoPointCalibrationGraphicPosition]  = "2-p calibration";
      words[restartCalibrationGraphicPosition] = "Reinitialiser valeurs";
      break;
    case portuguese:
      words[twoPointCalibrationGraphicPosition]  = "2-p calibration";
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
}
