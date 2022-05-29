
void UI_calibration() {
  byte numWords = 4;
  page = calibrateSensorsPage;
  print_text = true;
  tft.setTextSize(1);
  setTextColor(COLOR_MENU_TEXT);
  for (int i = false; i < numWords; i++) {
    pos_text[i] = CENTER;
  }
  switch (language) {
    case english:
      words[twoPointCalibrationGraphicPosition]  = convertStringToChar("2-p calibration");
      words[fineTuneCalibrationGraphicPosition]  = convertStringToChar("fine tune");
      words[autoCalibrationGraphicPosition]  = convertStringToChar("auto calibration");
      words[restartCalibrationGraphicPosition] = convertStringToChar("Reset values");
      break;
    case spanish:
      words[twoPointCalibrationGraphicPosition]  = convertStringToChar("2-p calibracion");
      words[fineTuneCalibrationGraphicPosition]  = convertStringToChar("ajuste fino");
      words[autoCalibrationGraphicPosition]  = convertStringToChar("auto calibracion");
      words[restartCalibrationGraphicPosition] = convertStringToChar("Reiniciar valores");
      break;
    case french:
      words[twoPointCalibrationGraphicPosition]  = convertStringToChar("2-p calibrage");
      words[fineTuneCalibrationGraphicPosition]  = convertStringToChar("affiner");
      words[autoCalibrationGraphicPosition]  = convertStringToChar("calibrage auto");
      words[restartCalibrationGraphicPosition] = convertStringToChar("Reinitialiser valeurs");
      break;
    case portuguese:
      words[twoPointCalibrationGraphicPosition]  = convertStringToChar("2-p calibracao");
      words[fineTuneCalibrationGraphicPosition]  = convertStringToChar("sintonia fina");
      words[autoCalibrationGraphicPosition]  = convertStringToChar("calibracao auto");
      words[restartCalibrationGraphicPosition] = convertStringToChar("Redefinir valores");
      break;
  }
  rectangles = numWords;
  graphics();
  drawHeading();
  bar_pos = true;
  ypos = graphicHeight(bar_pos - 1);
  while (!GPIORead(ENC_SWITCH)) {
    updateData();
  }
  vTaskDelay(debounceTime);
}
