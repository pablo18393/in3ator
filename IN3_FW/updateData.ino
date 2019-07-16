int updateData() {
  //check rotary encoder position
  EncNewPosition = myEncoderRead();
  EncMove = EncOldPosition - EncNewPosition;
  if (abs(EncMove) > 1) {
    if (EncMove > 0) {
      EncMove = 1;
    }
    else {
      EncMove = -1;
    }
  }
  lastEncoderPos[counter] = encoderpos[counter];
  EncOldPosition = EncNewPosition;

  if (page == advancedModePage || page == actuatorsProgressPage) {
    if (millis() - last_temp_update > temp_update_rate) {
      updateSensors();
      if (page == actuatorsProgressPage) {
        printStatus();
      }
    }
  }
  if ((page == mainMenuPage || page == advancedModePage) && !enableSet) {
    checkSetMessage();
  }
  checkSerialPort();
  checkNewPulsioximeterData();
  return EncMove;
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
    tft.drawFloat(previousTemperature[roomNTC], 1, temperatureX, temperatureY, textFontSize);
  }
  if (page != actuatorsProgressPage || controlHumidity) {
    drawHumidity();
  }
  updateTemp(numNTC);
  tft.setTextColor(COLOR_MENU_TEXT);
  if (page == advancedModePage || (page == actuatorsProgressPage && controlTemperature)) {
    tft.drawFloat(temperature[roomNTC], 1, temperatureX, temperatureY, textFontSize);
    previousTemperature[roomNTC] = temperature[roomNTC];
  }
  if (page == actuatorsProgressPage) {
    if (controlTemperature) {
      float previousTemperaturePercentage = temperaturePercentage;
      if (displayProcessPercentage) {
        drawRightNumber(temperaturePercentage, tft.width() / 2, temperatureY);
      }
      temperaturePercentage = 100 - ((desiredSkinTemp - temperature[roomNTC]) * 100 / (desiredSkinTemp - temperatureAtStart));
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
  last_temp_update = millis();
}

void checkSerialPort() {
  if (Serial1.available()) {
    switch (Serial1.read()) {
      case 'T':
        Serial1.println(temperature[0]);
        break;
      case 'P':
        Serial1.println("PING");
        break;
      case 'I':
        pinMode(PB1, INPUT_PULLUP);
        Serial1.println("INPUT");
        break;
      case 'A':
        Serial1.println(analogRead(PB1));
        /*        while (1) {
                  Serial1.println(analogRead(PB1));
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
            diffTemperature[roomNTC] = temperature[roomNTC] - readSerialData();
            temperature[roomNTC] -= diffTemperature[roomNTC];
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
        maxDACvalueHeater = readSerialData();
        Serial.print("Heater max PWM is: ");
        Serial.println(maxDACvalueHeater);        
        break;
      case 'R':
        nvic_sys_reset();
        break;
      case 'W':
        byte humidifierPWM;
        Serial.print("Humidifier working PWM is: ");
        humidifierPWM = readSerialData();
        analogWrite(HUMIDIFIER, humidifierPWM);
        Serial.println(humidifierPWM);
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
  Serial.print(millis() / 1000);
  Serial.print(";");
  Serial.print(desiredSkinTemp);
  Serial.print(";");
  Serial.print(heaterTempLimit);
  Serial.print(";");
  for (int i = 0; i < numTempSensors; i++) {
    Serial.print(temperature[i]);
    Serial.print(";");
  }
  Serial.print(desiredHeaterTemp);
  Serial.print(";");
  Serial.print(humidity);
  Serial.print(";");
  Serial.print(desiredRoomHum);
  Serial.print(";");
  Serial.print(heaterPower);
  Serial.print(";");
  Serial.println(PIDOutput[heaterNTC]);
}
