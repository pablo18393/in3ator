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

void UI_settings() {
  byte numWords = 6;
  page = settingsPage;
  print_text = true;
  tft.setTextSize(1);
  setTextColor(COLOR_MENU_TEXT);
  for (int i = false; i < numWords; i++) {
    pos_text[i] = LEFT_MARGIN;
  } 
  pos_text[setdefaultValuesGraphicPosition] = CENTER;
  pos_text[HWTestGraphicPosition] = CENTER;
  pos_text[calibrateGraphicPosition] = CENTER;
  switch (language) {
    case english:
      words[languageGraphicPosition] = convertStringToChar("Language");
      words[serialNumberGraphicPosition] = convertStringToChar("Serial number");
      words[setdefaultValuesGraphicPosition] = convertStringToChar("Set default values");
      words[calibrateGraphicPosition] = convertStringToChar("Calibration");
      break;
    case spanish:
      words[languageGraphicPosition] = convertStringToChar("Idioma");
      words[serialNumberGraphicPosition] = convertStringToChar("Serial number");
      words[setdefaultValuesGraphicPosition] = convertStringToChar("Fijar valores estandar");
      words[calibrateGraphicPosition] = convertStringToChar("Calibracion");
      break;
    case french:
      words[languageGraphicPosition] = convertStringToChar("Langue");
      words[serialNumberGraphicPosition] = convertStringToChar("Serial number");
      words[setdefaultValuesGraphicPosition] = convertStringToChar("Definir valeurs default");
      words[calibrateGraphicPosition] = convertStringToChar("Etalonner");
      break;
    case portuguese:
      words[languageGraphicPosition] = convertStringToChar("Idioma");
      words[serialNumberGraphicPosition] = convertStringToChar("Serial number");
      words[setdefaultValuesGraphicPosition] = convertStringToChar("Definir valores de fabrica");
      words[calibrateGraphicPosition] = convertStringToChar("Calibrar");
      break;
  }
  words[WifiENGraphicPosition] = convertStringToChar("WIFI");
  words[HWTestGraphicPosition] = convertStringToChar("HW Test");
  if (WiFi.status() == WL_CONNECTED ) {
    words[WifiENGraphicPosition] = convertStringToChar("WIFI IP->");
  }
  menu_rows = numWords;
  graphics();
  drawHeading();
  bar_pos = true;
  selected = false;
  ypos = graphicHeight(bar_pos - 1);
  while (!GPIORead(ENC_SWITCH)) {
    updateData();
  }
  vTaskDelay(debounceTime);
}
