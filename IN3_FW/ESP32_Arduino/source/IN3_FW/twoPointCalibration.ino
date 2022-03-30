
void firstPointCalibration() {
  byte numWords = 2;
  page = firstPointCalibrationPage;
  print_text = true;
  tft.setTextSize(1);
  setTextColor(COLOR_MENU_TEXT);
  for (int i = false; i < numWords; i++) {
    pos_text[i] = LEFT_MARGIN;
  }
  pos_text[setCalibrationGraphicPosition] = CENTER;
  switch (language) {
    case english:
      words[temperatureCalibrationGraphicPosition]  = "First point";
      words[setCalibrationGraphicPosition]  = "SET";
      break;
    case spanish:
      words[temperatureCalibrationGraphicPosition]  = "First point";
      words[setCalibrationGraphicPosition]  = "SET";
      break;
    case french:
      words[temperatureCalibrationGraphicPosition]  = "First point";
      words[setCalibrationGraphicPosition]  = "SET";
      break;
    case portuguese:
      words[temperatureCalibrationGraphicPosition]  = "First point";
      words[setCalibrationGraphicPosition]  = "SET";
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
  delay(debounceTime);
}

void secondPointCalibration() {
  byte numWords = 2;
  page = secondPointCalibrationPage;
  print_text = true;
  tft.setTextSize(1);
  setTextColor(COLOR_MENU_TEXT);
  for (int i = false; i < numWords; i++) {
    pos_text[i] = LEFT_MARGIN;
  }
  pos_text[setCalibrationGraphicPosition] = CENTER;
  switch (language) {
    case english:
      words[temperatureCalibrationGraphicPosition]  = "Second point";
      words[setCalibrationGraphicPosition]  = "SET";
      break;
    case spanish:
      words[temperatureCalibrationGraphicPosition]  = "Second point";
      words[setCalibrationGraphicPosition]  = "SET";
      break;
    case french:
      words[temperatureCalibrationGraphicPosition]  = "Second point";
      words[setCalibrationGraphicPosition]  = "SET";
      break;
    case portuguese:
      words[temperatureCalibrationGraphicPosition]  = "Second point";
      words[setCalibrationGraphicPosition]  = "SET";
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
  delay(debounceTime);
}
