
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
      words[twoPointCalibrationGraphicPosition]  = "2-p calibration";
      words[fineTuneCalibrationGraphicPosition]  = "fine tune";
      words[autoCalibrationGraphicPosition]  = "auto calibration";
      words[restartCalibrationGraphicPosition] = "Reset values";
      break;
    case spanish:
      words[twoPointCalibrationGraphicPosition]  = "2-p calibracion";
      words[fineTuneCalibrationGraphicPosition]  = "ajuste fino";
      words[autoCalibrationGraphicPosition]  = "auto calibracion";
      words[restartCalibrationGraphicPosition] = "Reiniciar valores";
      break;
    case french:
      words[twoPointCalibrationGraphicPosition]  = "2-p calibrage";
      words[fineTuneCalibrationGraphicPosition]  = "affiner";
      words[autoCalibrationGraphicPosition]  = "calibrage auto";
      words[restartCalibrationGraphicPosition] = "Reinitialiser valeurs";
      break;
    case portuguese:
      words[twoPointCalibrationGraphicPosition]  = "2-p calibracao";
      words[fineTuneCalibrationGraphicPosition]  = "sintonia fina";
      words[autoCalibrationGraphicPosition]  = "calibracao auto";
      words[restartCalibrationGraphicPosition] = "Redefinir valores";
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
