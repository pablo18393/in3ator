int updateData() {
  watchdogReload();
  GPRS_Handler();
  if (powerAlert) {
    logln("[ALARM] -> maximum power exceeded");
    powerAlert = false;
  }
  if (encPulseDetected) {
    logln("[ENCODER] -> Pushed");
    encPulseDetected = false;
  }
  if (millis() - lastDebugUpdate > debugUpdatePeriod) {
    if (temperaturePID.GetMode() == AUTOMATIC) {
      logln("[PID] -> Heater PWM output is: " + String (100 * PIDOutput / heaterMaxPWM) + "%");
    }
    if (humidityPID.GetMode() == AUTOMATIC) {
      logln("[PID] -> Humidifier output is: " + String (100 * humidityPIDOutput / WindowSize) + "%");
    }
    logln("[SENSORS] -> Current consumption is: " + String (currentConsumption) + " Amps");
    logln("[SENSORS] -> Baby temperature: " + String(temperature[babyNTC]) + "ºC");
    logln("[SENSORS] -> Air temperature: " + String(temperature[airNTC]) + "ºC");
    logln("[SENSORS] -> Floor temperature: " + String(temperature[digitalTempSensor]) + "ºC");
    logln("[SENSORS] -> Humidity: " + String(humidity) + "%");
    lastDebugUpdate = millis();
  }
  if (millis() - lastGraphicSensorsUpdate > sensorsUpdatePeriod) {
    updateRoomSensor();
    if (page == advancedModePage || page == actuatorsProgressPage) {
      updateDisplaySensors();
    }
    lastGraphicSensorsUpdate = millis();
  }
  if ((page == mainMenuPage || page == advancedModePage) && !enableSet) {
    checkSetMessage();
  }
}

void updateDisplaySensors() {
  if (page == advancedModePage || (page == actuatorsProgressPage)) {
    drawBabyTemperature();
    drawHumidity();
  }
  if (page == actuatorsProgressPage) {
    setTextColor(COLOR_MENU_TEXT);
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
        temperaturePercentage = false;
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
        humidityPercentage = false;
      }
      updateLoadingHumidityBar(int(previousHumidityPercentage), int(humidityPercentage));
    }
  }
}

void logln(String dataString) {
  log(String(millis() / 1000) + ": " + dataString + '\r' + '\n');
}

void log(String dataString) {
  if (SDCard) {
    /*
    GPIOWrite(SD_CS, LOW);
    GPIOWrite(TFT_CS, HIGH);
    // if the file is available, write to it:
    File dataFile = SD.open(logFile, FILE_WRITE);
    if (dataFile) {
      dataFile.print(day());
      dataFile.print("/");
      dataFile.print(month());
      dataFile.print("/");
      dataFile.print(year());
      dataFile.print("-");
      dataFile.print(hour());
      dataFile.print(":");
      dataFile.print(minute());
      dataFile.print(":");
      dataFile.print(second());
      dataFile.print(";");
      dataFile.print(dataString);
      dataFile.close();
      // print to the serial port too:
    }
    GPIOWrite(SD_CS, HIGH);
    GPIOWrite(TFT_CS, LOW);
    */
  }
  if (UARTDebug) {
    Serial.print(dataString);
  }
}

void GPRSLocalLog() {
  if (SDCard) {
    /*
    GPIOWrite(SD_CS, LOW);
    GPIOWrite(TFT_CS, HIGH);
    String dataString;
    dataString += temperature[babyNTC];
    dataString += ";";
    dataString += humidity;
    dataString += ";";
    // if the file is available, write to it:
    File dataFile = SD.open(GPRSFile, FILE_WRITE);
    if (dataFile) {
      dataFile.print(day());
      dataFile.print("/");
      dataFile.print(month());
      dataFile.print("/");
      dataFile.print(year());
      dataFile.print("-");
      dataFile.print(hour());
      dataFile.print(":");
      dataFile.print(minute());
      dataFile.print(":");
      dataFile.print(second());
      dataFile.print(";");
      dataFile.println(dataString);
      dataFile.close();
      // print to the serial port too:
      //logln(dataString);
    }
    GPIOWrite(SD_CS, HIGH);
    GPIOWrite(TFT_CS, LOW);
  */
  }
}
