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

void graphics() {
  setTextColor(COLOR_MENU_TEXT);
  if (!page) {
    tft.fillRect(width_select, height_heading, tft.width() - width_select, tft.height() - height_heading, COLOR_MENU);
  }
  else {
    tft.fillRect(0, height_heading, tft.width(), tft.height() - height_heading, COLOR_MENU);
  }
  if (print_text) {
    if (menu_rows) {
      tft.fillRect(0, height_heading, width_select, (tft.height() - height_heading) / menu_rows, COLOR_SELECTED);
    }
  }
  for (int i = 2; i <= menu_rows; i++) {
    if (menu_rows) {
      tft.fillRect(0, (tft.height() - height_heading) * (i - 1) / menu_rows + height_heading, width_select, (tft.height() - height_heading) / menu_rows, COLOR_BAR);
      tft.fillRect(0, (tft.height() - height_heading) * (i - 1) / menu_rows + height_heading - 1, tft.width(), width_indentation, WHITE);
    }
  }
  tft.drawRect(0, tft.height() - 1, width_select, tft.height() - 1, COLOR_MENU);
  if (print_text) {
    tft.setTextSize(1);
    for (int i = false; i < menu_rows; i++) {
      ypos = graphicHeight(i);
      if (!pos_text[i]) {
        drawString(words[i], width_select + side_gap, ypos, textFontSize);
      }
      else if (pos_text[i]) {
        drawCentreString(words[i], width_select + (tft.width() - width_select) / 2, ypos, textFontSize);
      }
      switch (page) {
        case mainMenuPage:
          switch (i) {
            case controlModeGraphicPosition:
              if (controlMode) {
                switch (language) {
                  case spanish:
                    textToWrite = convertStringToChar(cstring, "AIRE");
                    break;
                  case english:
                    textToWrite = convertStringToChar(cstring, "AIR");
                    break;
                  case french:
                    textToWrite = convertStringToChar(cstring, "AIR");
                    break;
                  case portuguese:
                    textToWrite = convertStringToChar(cstring, "AR");
                    break;
                }
                drawRightString(textToWrite, unitPosition, ypos, textFontSize);
              }
              else {
                switch (language) {
                  case spanish:
                    textToWrite = convertStringToChar(cstring, "PIEL");
                    break;
                  case english:
                    textToWrite = convertStringToChar(cstring, "SKIN");
                    break;
                  case french:
                    textToWrite = convertStringToChar(cstring, "PEAU");
                    break;
                  case portuguese:
                    textToWrite = convertStringToChar(cstring, "PELE");
                    break;
                }
                drawRightString(textToWrite, unitPosition, ypos, textFontSize);
              }
              break;
            case temperatureGraphicPosition:
              drawTemperatureUnits();
              drawRightString(initialSensorsValue, initialSensorPosition, temperatureY, textFontSize);
              break;
            case LEDGraphicPosition:
              if (jaundiceEnable) {
                drawRightString(convertStringToChar(cstring, "ON"), unitPosition, ypos, textFontSize);
              }
              else {
                drawRightString(convertStringToChar(cstring, "OFF"), unitPosition, ypos, textFontSize);
              }
              break;
            case humidityGraphicPosition:
              drawHumidityUnits();
              drawRightString(initialSensorsValue, initialSensorPosition, humidityY, textFontSize);
              break;
          }
          break;
        case settingsPage:
          switch (i) {
            case languageGraphicPosition:
              switch (language) {
                case spanish:
                  textToWrite = convertStringToChar(cstring, "SPA");
                  break;
                case english:
                  textToWrite = convertStringToChar(cstring, "ENG");
                  break;
                case french:
                  textToWrite = convertStringToChar(cstring, "FRA");
                  break;
                case portuguese:
                  textToWrite = convertStringToChar(cstring, "POR");
                  break;
              }
              drawRightString(textToWrite, unitPosition, ypos, textFontSize);
              break;
            case serialNumberGraphicPosition:
              drawRightNumber(serialNumber, unitPosition, ypos);
              break;
            case WifiENGraphicPosition:
              if (WIFI_EN) {
                drawRightString(convertStringToChar(cstring, "ON"), unitPosition, ypos, textFontSize);
                if (WiFi.status() == WL_CONNECTED ) {
                  drawCentreString(convertStringToChar(cstring, String(WiFi.localIP()[0]) + "." + String(WiFi.localIP()[1]) + "." + String(WiFi.localIP()[2]) + "." + String(WiFi.localIP()[3])), tft.width() / 2, ypos, textFontSize);
                }
              }
              else {
                drawRightString(convertStringToChar(cstring, "OFF"), unitPosition, ypos, textFontSize);
              }
          }
          break;
      }
    }
  }
}

void setSensorsGraphicPosition(int UI_page) {
  switch (UI_page) {
    case mainMenuPage:
      humidityX = tft.width() - 50;
      humidityY = graphicHeight(humidityGraphicPosition);
      temperatureX = tft.width() - 79;
      temperatureY = graphicHeight(temperatureGraphicPosition);
      break;
    case actuatorsProgressPage:
      barWidth = tft.width() / 4 * 2;
      barHeight = 20;
      tempBarPosX = tft.width() / 2;
      tempBarPosY = tft.height() / 3 - 11;
      humBarPosX = tempBarPosX;
      humBarPosY = 3 * tft.height() / 4 + 35;
      temperatureX = letter_width;
      temperatureY = tempBarPosY - barHeight / 2;
      humidityX = 2 * letter_width;
      humidityY = humBarPosY - barHeight / 2;
      separatorTopYPos = tft.height() / 3 + 5;
      separatorBotYPos = tft.height() * 2 / 3 + 10;
      break;
  }
}

void drawHeading() {
  tft.fillRect(0, 0, tft.width(), height_heading, COLOR_HEADING);
  if (page != mainMenuPage) {
    drawBack();
  }
  setTextColor(COLOR_MENU);
  drawCentreString(convertStringToChar(cstring, "in3_"), tft.width() / 2 - 2 * letter_width - 10, headint_text_height, textFontSize);
  drawCentreNumber(serialNumber, tft.width() / 2, headint_text_height);
  drawCentreString(convertStringToChar(cstring, FWversion), tft.width() - 4 * letter_width, headint_text_height, textFontSize);
}

void eraseBar() {
  if (menu_rows) {
    tft.fillRect(0, (tft.height() - height_heading) * (bar_pos - 1) / menu_rows + height_heading, width_select, (tft.height() - height_heading) / menu_rows, COLOR_BAR);
  }
}

void updateBar() {
  if (menu_rows) {
    tft.fillRect(0, (tft.height() - height_heading) * (bar_pos - 1) / menu_rows + height_heading, width_select, (tft.height() - height_heading) / menu_rows, COLOR_SELECTED);
    for (int i = 2; i <= menu_rows; i++) {
      tft.fillRect(0, (tft.height() - height_heading) * (i - 1) / menu_rows + height_heading - 1, tft.height(), width_indentation, WHITE); //mejorable
    }
  }
}

void drawBack() {
  tft.fillRect(0, 0, width_back, height_heading, COLOR_HEADING);
  tft.drawRect(0, 0, width_back, height_heading, BLACK);
  tft.fillTriangle(arrow_height, height_heading / 2, width_back / 2 , arrow_height, width_back / 2, height_heading - arrow_height, COLOR_ARROW);
  tft.fillRect(width_back / 2, height_heading / 2 - arrow_tail, width_back / 2 - arrow_height, arrow_tail, COLOR_ARROW);
  tft.fillRect(width_back / 2, height_heading / 2, width_back / 2 - arrow_height, arrow_tail, COLOR_ARROW);
}

void drawRightNumber(int n, int x, int i) {
  length = true;
  for (long k = 10; k <= n; k *= 10) {
    length++;
  }
  drawNumber(n, x - length * 14, i, textFontSize);
}

void loadlogo() {
  tft.setTextSize(1);
  tft.fillScreen(introBackColor);
  setTextColor(introTextColor);
  drawIntroMessage();
}

void drawIntroMessage() {
  byte numWords = 3;
  switch (language) {
    case english:
      words[0]  = convertStringToChar("Welcome to in3ator");
      words[1]  = convertStringToChar("");
      words[2]  = convertStringToChar("Saving lives");
      break;
    case spanish:
      words[0]  = convertStringToChar("Bienvenido a in3");
      words[1]  = convertStringToChar("");
      words[2]  = convertStringToChar("Salvando vidas");
      break;
    case french:
      words[0]  = convertStringToChar("Bienvenue a in3");
      words[1]  = convertStringToChar("");
      words[2]  = convertStringToChar("Sauver des vies");
      break;
    case portuguese:
      words[0]  = convertStringToChar("Bem-vindo ao");
      words[1]  = convertStringToChar("");
      words[2]  = convertStringToChar("Salvando vidas");
      break;
  }
  for (int i = false; i < numWords; i++) {
    drawCentreString(words[i], tft.width() / 2, tft.height() * (1 + i) / (2 + numWords) , textFontSize);
  }
}

/*
   Function pending to complete
*/
void drawHardwareErrorMessage(long error, bool criticalError) {
  char* messageToWrite[50];
  page = errorPage;
  tft.fillScreen(introBackColor);
  tft.setTextColor(introTextColor); //use tft. because tft.print is configured by it
  tft.setCursor(tft.width() / 4 - hexDigits(error) * 16, tft.height() / 5);
  tft.setTextSize(3);
  if (error) {
    tft.print("HW error:");
    tft.println(error, HEX);
    tft.println();
    tft.print(" ");
    switch (language) {
      case spanish:
        textToWrite = convertStringToChar(cstring, "Por favor contacta");
        break;
      case portuguese:
        textToWrite = convertStringToChar(cstring, "Por favor entre em contato");
        break;
      case english:
        textToWrite = convertStringToChar(cstring, "Please contact");
        break;
      case french:
        textToWrite = convertStringToChar(cstring, "S'il vous plait contactez");
        break;
    }
    tft.println(textToWrite);
    tft.setTextSize(2);
    tft.println("  medicalopenworld.org");
    if (criticalError) {
     tft.setTextSize(3);
     tft.println("CRITICAL ERROR");
     tft.println("CHECK WIRING");
    }
  }
  else {
    tft.print("SUCCESS :)");
  }
  tft.println();
  tft.println();
  tft.setTextSize(2);
  tft.print(" ");
  switch (language) {
    case spanish:
      textToWrite = convertStringToChar(cstring, "Presione para continuar");
      break;
    case portuguese:
      textToWrite = convertStringToChar(cstring, "Pressione para continuar");
      break;
    case english:
      textToWrite = convertStringToChar(cstring, "Press to continue");
      break;
    case french:
      textToWrite = convertStringToChar(cstring, "Appuyez pour continuer");
      break;
  }
  tft.println(textToWrite);
  while (GPIORead(ENC_SWITCH)) {
    updateData();
  }
}

void drawAlarmMessage(bool write, char *alertMessage) {
  if (write == DRAW) {
    setTextColor(COLOR_WARNING_TEXT);
    drawCentreString(alertMessage, width_select + (tft.width() - width_select) / 2, headint_text_height, textFontSize);
  }
  else {
    drawHeading();
  }
}

void drawHumidityUnits() {
  drawRightString(convertStringToChar(cstring, "/"), separatorPosition, ypos, textFontSize);
  drawRightString(convertStringToChar(cstring, "%"), unitPosition, ypos, textFontSize);
}

void drawTemperatureUnits() {
  drawRightString(convertStringToChar(cstring, "/"), separatorPosition, ypos, textFontSize);
  drawRightString(convertStringToChar(cstring, "C"), unitPosition, ypos, textFontSize);
}

void drawCentreNumber(int n, int x, int i) {
  drawNumber(n, x - decimalDigits(n) * 7, i, textFontSize);
}

int decimalDigits(long n) {
  int length = true;
  for (long k = 10; k <= n; k *= 10) {
    length++;
  }
  return (length);
}

int hexDigits(long n) {
  int length = true;
  for (long k = 16; k <= n; k *= 16) {
    length++;
  }
  return (length);
}

void drawStop() {
  if (display_drawStop) {
    switch (language) {
      case spanish:
        textToWrite = convertStringToChar(cstring, "Pulsa 2 seg para salir");
        break;
      case english:
        textToWrite = convertStringToChar(cstring, "Press 2 sec to go back");
        break;
      case french:
        textToWrite = convertStringToChar(cstring, "appuyez 2 sec pour voler");
        break;
      case portuguese:
        textToWrite = convertStringToChar(cstring, "Pressione 2 segundos para sair");
        break;
    }
    drawCentreString(textToWrite, tft.width() / 2, tft.height() - letter_height, textFontSize);
  }
}

int graphicHeight(int position) {
  if (menu_rows) {
    return ((tft.height() - height_heading) / (2 * menu_rows) + position * (tft.height() - height_heading) / (menu_rows) + letter_height);
  }
  return false;
}

void drawSelectedTemperature() {
  float temperatureToDraw, previousTemperatureDrawn;
  if (controlMode) {
    previousTemperatureDrawn = previousTemperature[airSensor];
    temperatureToDraw = temperature[airSensor];
  }
  else {
    previousTemperatureDrawn = previousTemperature[skinSensor];
    temperatureToDraw = temperature[skinSensor];
  }
  setTextColor(COLOR_MENU);
  drawFloat(previousTemperatureDrawn, 1, temperatureX, temperatureY, textFontSize);
  setTextColor(COLOR_MENU_TEXT);
  drawFloat(temperatureToDraw, 1, temperatureX, temperatureY, textFontSize);
  if (controlMode) {
    previousTemperature[airSensor] = temperatureToDraw;
  }
  else {
    previousTemperature[skinSensor] = temperatureToDraw;
  }
}

void drawUnselectedTemperature() {
  float temperatureToDraw, previousTemperatureDrawn;
  if (!controlMode) {
    previousTemperatureDrawn = previousTemperature[airSensor];
    temperatureToDraw = temperature[airSensor];
  }
  else {
    previousTemperatureDrawn = previousTemperature[skinSensor];
    temperatureToDraw = temperature[skinSensor];
  }
  tft.setTextColor(COLOR_MENU);
  drawFloat(previousTemperatureDrawn, 1, tft.width() / 2 - 20, tft.height() / 2 + 10, textFontSize);
  tft.setTextColor(COLOR_MENU_TEXT);
  drawFloat(temperatureToDraw, 1, tft.width() / 2 - 20, tft.height() / 2 + 10, textFontSize);
  if (!controlMode) {
    previousTemperature[airSensor] = temperatureToDraw;
  }
  else {
    previousTemperature[skinSensor] = temperatureToDraw;
  }
}

void drawHumidity() {
  setTextColor(COLOR_MENU);
  drawCentreNumber(previousHumidity, humidityX, humidityY);
  setTextColor(COLOR_MENU_TEXT);
  previousHumidity = humidity;
  drawCentreNumber(previousHumidity, humidityX, humidityY);
}

void drawStartMessage() {
  if (enableSet) {
    setTextColor(COLOR_MENU);
    drawCentreString(helpMessage, width_select + (tft.width() - width_select) / 2, getYpos(goToProcessRow), textFontSize);
    setTextColor(COLOR_MENU_TEXT);
    switch (language) {
      case spanish:
        words[startGraphicPosition] = convertStringToChar(cstring, "Empezar");
        break;
      case english:
        words[startGraphicPosition] = convertStringToChar(cstring, "Start");
        break;
      case french:
        words[startGraphicPosition] = convertStringToChar(cstring, "Debut");
        break;
      case portuguese:
        words[startGraphicPosition] = convertStringToChar(cstring, "Comecar");
        break;
    }
    drawCentreString(words[startGraphicPosition], width_select + (tft.width() - width_select) / 2, getYpos(goToProcessRow), textFontSize);
  }
}

void drawActuatorsSeparators() {
  barThickness = 3;
  tft.fillRect(0, separatorTopYPos, tft.width(), barThickness, COLOR_FRAME_BAR);
  tft.fillRect(0, separatorBotYPos, tft.width(), barThickness, COLOR_FRAME_BAR);
}

void printLoadingTemperatureBar() {
  barThickness = 3;
  drawFloat(desiredControlTemperature, 1, tft.width() - 5 * letter_width, temperatureY, textFontSize);
  for (int i = true; i <= barThickness; i++) {
    tft.drawRect(tempBarPosX - barWidth / 2 - i, tempBarPosY - barHeight / 2 - i, barWidth + i * 2, barHeight + i * 2, COLOR_FRAME_BAR);
  }
}

void printLoadingHumidityBar() {
  barThickness = 3;
  drawFloat(desiredControlHumidity, 1, humBarPosX + barWidth / 2 + 10, humidityY, textFontSize);
  for (int i = true; i <= barThickness; i++) {
    tft.drawRect(humBarPosX - barWidth / 2 - i, humBarPosY - barHeight / 2 - i, barWidth + i * 2, barHeight + i * 2, COLOR_FRAME_BAR);
  }
}

void updateLoadingTemperatureBar(float prev, float actual) {
  if (prev != actual) {
    float diff = (actual - prev) / 100;
    int color;
    float barX;
    int barY, barDiffWidth;
    barX = tempBarPosX - (barWidth / 2) * (1 - prev / 50);
    barY = tempBarPosY - barHeight / 2;
    barDiffWidth = barWidth * abs(diff) + 1;
    if (diff > 0) {
      color = COLOR_LOADING_BAR;
    }
    else {
      color = COLOR_MENU;
      barX -= barDiffWidth - 1;
    }
    tft.fillRect(barX, barY, barDiffWidth, barHeight, color);
    if (displayProcessPercentage) {
      setTextColor(COLOR_MENU);
      drawRightNumber(prev, tft.width() / 2, temperatureY);
      setTextColor(COLOR_MENU_TEXT);
      drawRightNumber(actual, tft.width() / 2, temperatureY);
      drawCentreString(convertStringToChar(cstring, "%"), tft.width() / 2 + 14, temperatureY , textFontSize);
    }
  }
}

void updateLoadingHumidityBar(float prev, float actual) {
  if (prev != actual) {
    float diff = (actual - prev) / 100;
    int color;
    float barX;
    int barY, barDiffWidth;
    barX = humBarPosX - (barWidth / 2) * (1 - prev / 50);
    barY = humBarPosY - barHeight / 2;
    barDiffWidth = barWidth * abs(diff) + 1;
    if (diff > 0) {
      color = COLOR_LOADING_BAR;
    }
    else {
      color = COLOR_MENU;
      barX -= barDiffWidth - 1;
    }
    tft.fillRect(barX, barY, barDiffWidth, barHeight, color);
    if (displayProcessPercentage) {
      setTextColor(COLOR_MENU);
      drawRightNumber(prev, tft.width() / 2, humidityY);
      setTextColor(COLOR_MENU_TEXT);
      drawRightNumber(actual, tft.width() / 2, humidityY);
      drawCentreString(convertStringToChar(cstring, "%"), tft.width() / 2 + 14, humidityY, textFontSize);
    }
  }
}

void blinkGoBackMessage() {
  if (millis() - blinking > 1000) {
    blinking = millis();
    state_blink = !state_blink;
    if (state_blink) {
      setTextColor(ILI9341_ORANGE);
    }
    else {
      setTextColor(COLOR_MENU);
      blinking += 400;
    }
    drawStop();
  }
}

int16_t drawString(char *string, int16_t poX, int16_t poY, int16_t size)
{
  int16_t sumX = 0;
  int16_t xPlus = 0;
  int16_t width = 8;
  int16_t gap = -2;

  while (*string)
  {
    tft.drawChar(poX, poY, *string, screenTextColor, screenTextBackgroundColor, size);
    *string++;
    poX += (width + gap) * size;                        /* Move cursor right       */
  }
  return sumX;
}

int16_t drawCentreString(char *string, int16_t dX, int16_t poY, int16_t size)
{
  int16_t sumX = 0;
  int16_t len = 0;
  char *pointer = string;
  char ascii;
  int16_t width = 8;
  int16_t gap = -2;

  while (*pointer)
  {
    ascii = *pointer;
    len += (width + gap);
    *pointer++;
  }
  len = len * size;
  int16_t poX = dX - len / 2;
  if (poX < 0) poX = 0;

  while (*string)
  {
    tft.drawChar(poX, poY, *string, screenTextColor, screenTextBackgroundColor, size);
    *string++;
    poX += (width + gap) * size;                        /* Move cursor right       */
  }
  return sumX;
}

int16_t drawRightString(char *string, int16_t dX, int16_t poY, int16_t size)
{
  int16_t sumX = 0;
  int16_t len = 0;
  char *pointer = string;
  char ascii;
  int16_t width = 8;
  int16_t gap = -2;

  while (*pointer)
  {
    ascii = *pointer;
    len += (width + gap);
    *pointer++;
  }

  len = len * size;
  int16_t poX = dX - len;

  if (poX < 0) poX = 0;

  while (*string)
  {
    tft.drawChar(poX, poY, *string, screenTextColor, screenTextBackgroundColor, size);
    *string++;
    poX += (width + gap) * size;                        /* Move cursor right       */
  }

  return sumX;
}

int16_t drawNumber(long long_num, int16_t poX, int16_t poY, int16_t size)
{
  char tmp[10];
  if (long_num < 0) sprintf(tmp, "%li", long_num);
  else sprintf(tmp, "%lu", long_num);
  return drawString(tmp, poX, poY, size);
}

void setTextColor(int16_t color) {
  screenTextColor = color;
}

void setBackgroundColor(int16_t color) {
  screenTextBackgroundColor = color;
}

int16_t drawFloat(float floatNumber, int16_t decimal, int16_t poX, int16_t poY, int16_t size)
{
  unsigned long temp = 0;
  float decy = 0.0;
  float rounding = 0.5;
  float eep = 0.000001;
  int16_t sumX    = 0;
  int16_t xPlus   = 0;
  char negativeSymbol[] = "-";
  char decimalSymbol[] = ".";
  int16_t width = 8;
  int16_t gap = -2;
  int r = 1;


  if (floatNumber - 0.0 < eep)    // floatNumber < 0
  {
    drawString(negativeSymbol, poX, poY, size);
    floatNumber = -floatNumber;
    poX  += (width + gap) * size;
    sumX += (width + gap) * size;
  }

  for (unsigned char i = 0; i < decimal; ++i)
  {
    rounding /= 10.0;
  }

  floatNumber += rounding;

  temp = (long)floatNumber;

  xPlus = drawNumber(temp, poX, poY, size);

  while (temp > 9) {
    temp /= 10;
    r++;
  }
  temp = (long)floatNumber;
  poX  += (width + gap) * size * r;
  sumX += (width + gap) * size;

  if (decimal > 0)
  {
    drawString(decimalSymbol, poX, poY, size);
    poX  += (width + gap) * size;                            /* Move cursor right            */
    sumX += (width + gap) * size;
  }
  else
  {
    return sumX;
  }

  decy = floatNumber - temp;
  for (unsigned char i = 0; i < decimal; i++)
  {
    decy *= 10;                                /* for the next decimal         */
    temp = decy;                               /* get the decimal              */
    drawNumber(temp, poX, poY, size);

    poX += (width + gap) * size;                              /* Move cursor right            */
    sumX += (width + gap) * size;
    decy -= temp;
  }
  return sumX;
}

char * convertStringToChar (String input) {
  char * cstr = new char [input.length() + 1];
  strcpy (cstr, input.c_str());
  return cstr;
}

char * convertStringToChar (char* arrayInput, String input) {
  strcpy (arrayInput, input.c_str());
  return arrayInput;
}
