int updateData() {
  if (millis() - lastSensorsUpdate > sensorsUpdateRate) {
    if (page == advancedModePage || page == actuatorsProgressPage) {
      updateSensors();
      if (page == actuatorsProgressPage) {
        printStatus();
      }
      lastSensorsUpdate = millis();
    }
    else {
      updateHumidity();
      updateTemp(numNTC);
    }
  }
  if ((page == mainMenuPage || page == advancedModePage) && !enableSet) {
    checkSetMessage();
  }
  checkSerialPort();
  checkNewPulsioximeterData();
  /*
    if (!GSM.postRPS && millis()>20000) {
    GSM.postRPS = 1;
    postGSMVariables();
    }
  */
}

void checkNewPulsioximeterData() {
  if (pulsioximeterCounter[pulsioximeterDrawn] = !pulsioximeterCounter[pulsioximeterSampled]) {
    drawPulsioximeter();
    if (pulsioximeterCounter[pulsioximeterDrawn] == maxPulsioximeterSamples) {
      pulsioximeterCounter[pulsioximeterDrawn] = 0;
    }
    else {
      pulsioximeterCounter[pulsioximeterDrawn]++;
    }
  }
}

void updateSensors() {
  tft.setTextColor(COLOR_MENU);
  if (page == advancedModePage || (page == actuatorsProgressPage && controlTemperature)) {
    tft.drawFloat(previousTemperature[babyNTC], 1, temperatureX, temperatureY, textFontSize);
  }
  if (page != actuatorsProgressPage && page != mainMenuPage || controlHumidity) {
    drawHumidity();
  }
  updateTemp(numNTC);
  tft.setTextColor(COLOR_MENU_TEXT);
  if (page == advancedModePage || (page == actuatorsProgressPage && controlTemperature)) {
    tft.drawFloat(temperature[babyNTC], 1, temperatureX, temperatureY, textFontSize);
    previousTemperature[babyNTC] = temperature[babyNTC];
  }
  if (page == actuatorsProgressPage) {
    if (controlTemperature) {
      float previousTemperaturePercentage = temperaturePercentage;
      if (displayProcessPercentage) {
        drawRightNumber(temperaturePercentage, tft.width() / 2, temperatureY);
      }
      temperaturePercentage = 100 - ((desiredSkinTemp - temperature[babyNTC]) * 100 / (desiredSkinTemp - temperatureAtStart));
      if (temperaturePercentage > 99) {
        temperaturePercentage = 100;
      }
      if (temperaturePercentage < 0) {
        temperaturePercentage = 0;
      }

      updateLoadingTemperatureBar(int(previousTemperaturePercentage), int(temperaturePercentage));
    }
    if (controlHumidity) {
      float previousHumidityPercentage = humidityPercentage;
      if (displayProcessPercentage) {
        drawRightNumber(humidityPercentage, tft.width() / 2, humidityY);
      }
      humidityPercentage = 100 - ((desiredRoomHum - humidity) * 100 / (desiredRoomHum - humidityAtStart));
      if (humidityPercentage > 99) {
        humidityPercentage = 100;
      }
      if (humidityPercentage < 0) {
        humidityPercentage = 0;
      }

      updateLoadingHumidityBar(int(previousHumidityPercentage), int(humidityPercentage));
    }
  }
}

void checkSerialPort() {
  if (Serial.available()) {
    switch (Serial.read()) {
      case 'T':
        Serial.println(temperature[0]);
        break;
      case 'P':
        Serial.println("PING");
        break;
      case 'I':
        pinMode(PB1, INPUT_PULLUP);
        Serial.println("INPUT");
        break;
      case 'A':
        Serial.println(analogRead(PB1));
        /*        while (1) {
                  Serial.println(analogRead(PB1));
                  delay(15);
                  }
        */
        break;
    }
  }
  if (Serial.available()) {
    switch (Serial.read()) {
      case 'T':
        switch (Serial.read()) {
          case 'C':
            diffTemperature[babyNTC] = temperature[babyNTC] - readSerialData();
            temperature[babyNTC] -= diffTemperature[babyNTC];
            break;
          case 'H':
            diffTemperature[heaterNTC] = temperature[heaterNTC] - readSerialData();
            temperature[heaterNTC] -= diffTemperature[heaterNTC];
            break;
        }
        break;
      case 'H':
        diffHumidity = humidity - readSerialData();
        humidity -= diffHumidity;
        break;
      case 'P':
        HeatermaxPWM = readSerialData();
        log("Heater max PWM is: ");
        logln(String(HeatermaxPWM));
        break;
      case 'R':
        nvic_sys_reset();
        break;
      case 'W':
        byte humidifierPWM;
        log("Humidifier working PWM is: ");
        humidifierPWM = readSerialData();
        analogWrite(HUMIDIFIER, humidifierPWM);
        logln(String(humidifierPWM));
        break;
    }
  }
}

int readSerialData() {
  int serialData = 0;
  delay(15);
  while (Serial.available()) {
    serialData *= 10;
    serialData += Serial.read() - 48;
  }
  return (serialData);
}

void printStatus() {
  log(millis() / 1000);
  log(";");
  log(desiredSkinTemp);
  log(";");
  log(maxHeaterTemp);
  log(";");
  for (int i = 0; i < numTempSensors; i++) {
    log(temperature[i]);
    log(";");
  }
  log(desiredHeaterTemp);
  log(";");
  log(humidity);
  log(";");
  log(desiredRoomHum);
  log(";");
  log(heaterPower);
  log(";");
  logln(String(PIDOutput[heaterNTC]));
}
