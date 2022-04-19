void graphics() {
  setTextColor(COLOR_MENU_TEXT);
  if (!page) {
    tft.fillRect(width_select, height_heading, tft.width() - width_select, tft.height() - height_heading, COLOR_MENU);
  }
  else {
    tft.fillRect(0, height_heading, tft.width(), tft.height() - height_heading, COLOR_MENU);
  }
  if (print_text) {
    tft.fillRect(0, height_heading, width_select, (tft.height() - height_heading) / rectangles, COLOR_SELECTED);
  }
  for (int i = 2; i <= rectangles; i++) {
    tft.fillRect(0, (tft.height() - height_heading) * (i - 1) / rectangles + height_heading, width_select, (tft.height() - height_heading) / rectangles, COLOR_BAR);
    tft.fillRect(0, (tft.height() - height_heading) * (i - 1) / rectangles + height_heading - 1, tft.width(), width_indentation, WHITE);
  }
  tft.drawRect(0, tft.height() - 1, width_select, tft.height() - 1, COLOR_MENU);
  if (print_text) {
    tft.setTextSize(1);
    text_size = true;
    for (int i = false; i < rectangles; i++) {
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
                    textToWrite = "AIRE";
                    break;
                  case english:
                    textToWrite = "AIR";
                    break;
                  case french:
                    textToWrite = "AIR";
                    break;
                  case portuguese:
                    textToWrite = "AR";
                    break;
                }
                drawRightString(textToWrite, unitPosition, ypos, textFontSize);
              }
              else {
                switch (language) {
                  case spanish:
                    textToWrite = "PIEL";
                    break;
                  case english:
                    textToWrite = "SKIN";
                    break;
                  case french:
                    textToWrite = "PEAU";
                    break;
                  case portuguese:
                    textToWrite = "PELE";
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
                drawRightString("ON", unitPosition, ypos, textFontSize);
              }
              else {
                drawRightString("OFF", unitPosition, ypos, textFontSize);
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
            case autoLockGraphicPosition:
              if (autoLock) {
                switch (language) {
                  case spanish:
                    textToWrite = "SI";
                    break;
                  case english:
                    textToWrite = "YES";
                    break;
                  case french:
                    textToWrite = "OUI";
                    break;
                  case portuguese:
                    textToWrite = "SIM";
                    break;
                }
              }
              else {
                switch (language) {
                  case spanish:
                    textToWrite = "NO";
                    break;
                  case english:
                    textToWrite = "NO";
                    break;
                  case french:
                    textToWrite = "PAS";
                    break;
                  case portuguese:
                    textToWrite = "NAO";
                    break;
                }
              }
              drawRightString(textToWrite, unitPosition, ypos, textFontSize);
              break;
            case languageGraphicPosition:
              switch (language) {
                case spanish:
                  textToWrite = "SPA";
                  break;
                case english:
                  textToWrite = "ENG";
                  break;
                case french:
                  textToWrite = "FRA";
                  break;
                case portuguese:
                  textToWrite = "POR";
                  break;
              }
              drawRightString(textToWrite, unitPosition, ypos, textFontSize);
              break;
            case controlAlgorithmGraphicPosition:
              if (controlAlgorithm) {
                textToWrite = "PID";
              }
              else {
                textToWrite = "BASIC";
              }
              drawRightString(textToWrite, unitPosition, ypos, textFontSize);
              break;
            case serialNumberGraphicPosition:
              drawRightNumber(serialNumber, 280, ypos);
              break;
            case WifiENGraphicPosition:
              if (WIFI_EN) {
                drawRightString("ON", unitPosition, ypos, textFontSize);
              }
              else {
                drawRightString("OFF", unitPosition, ypos, textFontSize);
              }
          }
          break;
      }
    }
  }
}

void setSensorsGraphicPosition() {
  switch (page) {
    case mainMenuPage:
      humidityX = tft.width() - 50;
      humidityY = graphicHeight(humidityGraphicPosition);
      temperatureX = tft.width() - 79;
      temperatureY = graphicHeight(temperatureGraphicPosition);
      LEDXPos = tft.width() - letter_width * 2;
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
  drawCentreString("in3_", tft.width() / 2 - 2 * letter_width - 10, height_heading / 5, textFontSize);
  drawCentreNumber(serialNumber, tft.width() / 2, height_heading / 5);
  drawCentreString(FWversion, tft.width() - 4 * letter_width, height_heading / 5, textFontSize);
}

void eraseBar() {
  tft.fillRect(0, (tft.height() - height_heading) * (bar_pos - 1) / rectangles + height_heading, width_select, (tft.height() - height_heading) / rectangles, COLOR_BAR);
}

void updateBar() {
  tft.fillRect(0, (tft.height() - height_heading) * (bar_pos - 1) / rectangles + height_heading, width_select, (tft.height() - height_heading) / rectangles, COLOR_SELECTED);
  for (int i = 2; i <= rectangles; i++) {
    tft.fillRect(0, (tft.height() - height_heading) * (i - 1) / rectangles + height_heading - 1, tft.height(), width_indentation, WHITE); //mejorable
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
      words[0]  = "Welcome to in3ator";
      words[1]  = "";
      words[2]  = "Saving lives";
      break;
    case spanish:
      words[0]  = "Bienvenido a in3";
      words[1]  = "";
      words[2]  = "Salvando vidas";
      break;
    case french:
      words[0]  = "Bienvenue a in3";
      words[1]  = "";
      words[2]  = "Sauver des vies";
      break;
    case portuguese:
      words[0]  = "Bem-vindo ao";
      words[1]  = "";
      words[2]  = "Salvando vidas";
      break;
  }
  for (int i = false; i < numWords; i++) {
    drawCentreString(words[i], tft.width() / 2, tft.height() * (1 + i) / (2 + numWords) , textFontSize);
  }
}

/*
   Function pending to complete
*/
void drawHardwareErrorMessage(long error) {
  char* messageToWrite[50];
  page = errorPage;
  tft.fillScreen(introBackColor);
  tft.setTextColor(introTextColor); //use tft. because tft.print is configured by it
  tft.setCursor(tft.width() / 4 - hexDigits(error) * 16, tft.height() / 5);
  tft.setTextSize(3);
  tft.print("HW error:");
  tft.println(error, HEX);
  tft.println();
  tft.print(" ");
  switch (language) {
    case spanish:
      textToWrite = "Por favor contacta";
      break;
    case portuguese:
      textToWrite = "Por favor entre em contato";
      break;
    case english:
      textToWrite = "Please contact";
      break;
    case french:
      textToWrite = "S'il vous plait contactez";
      break;
  }
  tft.println(textToWrite);
  tft.setTextSize(2);
  tft.println("  medicalopenworld.org");
  tft.setTextSize(2);
  tft.println();
  tft.println();
  tft.print(" ");
  switch (language) {
    case spanish:
      textToWrite = "Presione para continuar";
      break;
    case portuguese:
      textToWrite = "Pressione para continuar";
      break;
    case english:
      textToWrite = "Press to continue";
      break;
    case french:
      textToWrite = "Appuyez pour continuer";
      break;
  }
  tft.println(textToWrite);
  while (GPIORead(ENC_SWITCH)) {
    updateData();
  }
}

void drawAlarmMessage(bool write, byte roomVariable) {
  int alarmHeight;
  if (write == DRAW) {
    setTextColor(COLOR_WARNING_TEXT);
  }
  else {
    setTextColor(COLOR_MENU);
  }
  switch (roomVariable) {
    case AIR_THERMAL_CUTOUT_ALARM:
    case SKIN_THERMAL_CUTOUT_ALARM:
      helpMessage = "THERMAL CUTOUT ALARM";
      alarmHeight = 105;
      break;
    case TEMPERATURE_ALARM:
      helpMessage = "TEMPERATURE ALARM";
      alarmHeight = 105;
      break;
    case HUMIDITY_ALARM:
      helpMessage = "HUMIDITY ALARM";
      alarmHeight = 140;
      break;
  }

  drawCentreString(helpMessage, width_select + (tft.width() - width_select) / 2, alarmHeight, textFontSize);
}

void drawHumidityUnits() {
  drawRightString("/", separatorPosition, ypos, textFontSize);
  drawRightString("%", unitPosition, ypos, textFontSize);
}

void drawTemperatureUnits() {
  drawRightString("/", separatorPosition, ypos, textFontSize);
  drawRightString("C", unitPosition, ypos, textFontSize);
}

void drawCentreNumber(int n, int x, int i) {
  if (text_size == 2) {
    drawNumber(n, x - decimalDigits(n) * 27, i, 6);
  }
  else {
    drawNumber(n, x - decimalDigits(n) * text_size * 7, i, textFontSize);
  }
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
        textToWrite = "Pulsa 2 seg para salir";
        break;
      case english:
        textToWrite = "Press 2 sec to go back";
        break;
      case french:
        textToWrite = "appuyez 2 sec pour voler";
        break;
      case portuguese:
        textToWrite = "Pressione 2 segundos para sair";
        break;
    }
    drawCentreString(textToWrite, tft.width() / 2, tft.height() - letter_height, textFontSize);
  }
}

int graphicHeight(int position) {
  return ((tft.height() - height_heading) / (2 * rectangles) + position * (tft.height() - height_heading) / (rectangles) + letter_height);
}

void drawSelectedTemperature() {
  float temperatureToDraw, previousTemperatureDrawn;
  if (controlMode) {
    previousTemperatureDrawn = previousTemperature[airSensor];
    temperatureToDraw = temperature[airSensor];
  }
  else {
    previousTemperatureDrawn = previousTemperature[babySensor];
    temperatureToDraw = temperature[babySensor];
  }
  setTextColor(COLOR_MENU);
  drawFloat(previousTemperatureDrawn, 1, temperatureX, temperatureY, textFontSize);
  setTextColor(COLOR_MENU_TEXT);
  drawFloat(temperatureToDraw, 1, temperatureX, temperatureY, textFontSize);
  if (controlMode) {
    previousTemperature[airSensor] = temperatureToDraw;
  }
  else {
    previousTemperature[babySensor] = temperatureToDraw;
  }
}

void drawUnselectedTemperature() {
  float temperatureToDraw, previousTemperatureDrawn;
  if (!controlMode) {
    previousTemperatureDrawn = previousTemperature[airSensor];
    temperatureToDraw = temperature[airSensor];
  }
  else {
    previousTemperatureDrawn = previousTemperature[babySensor];
    temperatureToDraw = temperature[babySensor];
  }
  tft.setTextColor(COLOR_MENU);
  drawFloat(previousTemperatureDrawn, 1, tft.width() / 2 - 20, tft.height() / 2 + 10, textFontSize);
  tft.setTextColor(COLOR_MENU_TEXT);
  drawFloat(temperatureToDraw, 1, tft.width() / 2 - 20, tft.height() / 2 + 10, textFontSize);
  if (!controlMode) {
    previousTemperature[airSensor] = temperatureToDraw;
  }
  else {
    previousTemperature[babySensor] = temperatureToDraw;
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
        words[startGraphicPosition] = "Empezar";
        break;
      case english:
        words[startGraphicPosition] = "Start";
        break;
      case french:
        words[startGraphicPosition] = "Debut";
        break;
      case portuguese:
        words[startGraphicPosition] = "Comecar";
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
  drawFloat(desiredControlTemp, 1, tft.width() - 5 * letter_width, temperatureY, textFontSize);
  for (int i = true; i <= barThickness; i++) {
    tft.drawRect(tempBarPosX - barWidth / 2 - i, tempBarPosY - barHeight / 2 - i, barWidth + i * 2, barHeight + i * 2, COLOR_FRAME_BAR);
  }
}

void printLoadingHumidityBar() {
  barThickness = 3;
  drawFloat(desiredRoomHum, 1, humBarPosX + barWidth / 2 + 10, humidityY, textFontSize);
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
      drawCentreString("%", tft.width() / 2 + 14, temperatureY , textFontSize);
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
      drawCentreString("%", tft.width() / 2 + 14, humidityY, textFontSize);
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
