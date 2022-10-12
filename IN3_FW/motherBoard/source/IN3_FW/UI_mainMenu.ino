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

void UI_mainMenu() {
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
      words[controlModeGraphicPosition]  = convertStringToChar("Modo de control");
      if (controlMode) {
        words[temperatureGraphicPosition]  = convertStringToChar("Temp aire");
      }
      else {
        words[temperatureGraphicPosition]  = convertStringToChar("Temp piel");
      }
      words[humidityGraphicPosition] = convertStringToChar("Humedad");
      words[LEDGraphicPosition] = convertStringToChar("Fototerapia");
      break;
    case english:
      words[controlModeGraphicPosition]  = convertStringToChar("Control mode");
      if (controlMode) {
        words[temperatureGraphicPosition]  = convertStringToChar("Air temp");
      }
      else {
        words[temperatureGraphicPosition]  = convertStringToChar("Skin temp");
      }
      words[humidityGraphicPosition] = convertStringToChar("Humidity");
      words[LEDGraphicPosition] = convertStringToChar("Phototherapy");
      break;
    case french:
      words[controlModeGraphicPosition]  = convertStringToChar("Mode de controle");
      if (controlMode) {
        words[temperatureGraphicPosition]  = convertStringToChar("Temp air");
      }
      else {
        words[temperatureGraphicPosition]  = convertStringToChar("Temp peau");
      }
      words[humidityGraphicPosition] = convertStringToChar("Humidite");
      words[LEDGraphicPosition] = convertStringToChar("Phototherapie");
      break;
    case portuguese:
      words[controlModeGraphicPosition]  = convertStringToChar("Modo de controle");
      if (controlMode) {
        words[temperatureGraphicPosition]  = convertStringToChar("Temp do ar");
      }
      else {
        words[temperatureGraphicPosition]  = convertStringToChar("Temp pele");
      }
      words[humidityGraphicPosition] = convertStringToChar("Umidade");
      words[LEDGraphicPosition] = convertStringToChar("Fototerapia");
      break;
  }
  words[startGraphicPosition] = convertStringToChar("");
  menu_rows = numWords;
  setSensorsGraphicPosition(page);
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
  if (desiredControlTemperature < minDesiredTemp[controlMode] || desiredControlTemperature > maxDesiredTemp[controlMode]) {
    desiredControlTemperature = presetTemp[controlMode];
  }
  while (!GPIORead(ENC_SWITCH)) {
    updateData();
  }
  vTaskDelay(debounceTime);
}
