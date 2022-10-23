/*
  MIT License

  Copyright (c) 2022 Medical Open World, Pablo Sánchez Bergasa

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

*/

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
  menu_rows = numWords;
  graphics(page, language, print_text, menu_rows, NULL, NULL);
  drawHeading(page, serialNumber, FWversion);
  bar_pos = true;
  ypos = graphicHeight(bar_pos - 1);
  while (!digitalRead(ENC_SWITCH)) {
    updateData();
  }
  vTaskDelay(debounceTime);
}
