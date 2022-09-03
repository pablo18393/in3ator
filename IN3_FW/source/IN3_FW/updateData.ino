long lastHumToggle;
bool humToggle;

void updateData() {
  loopCounts++;
  buzzerHandler();
  watchdogReload();
  OTAHandler();
  sensorsHandler();
  GPRS_Handler();
  if (page != autoCalibrationPage) {
    securityCheck();
  }
  if (powerAlert) {
    logln("[ALARM] -> maximum power exceeded");
    powerAlert = false;
  }
  if (encPulseDetected && GPIORead(ENC_SWITCH)) {
    encPulseDetected = false;
  }
  if (millis() - lastDebugUpdate > debugUpdatePeriod) {
    if (airControlPID.GetMode() == AUTOMATIC) {
      logln("[PID] -> Heater PWM output is: " + String (100 * HeaterPIDOutput / BUZZER_MAX_PWR) + "%");
    }
    if (skinControlPID.GetMode() == AUTOMATIC) {
      logln("[PID] -> Heater PWM output is: " + String (100 * HeaterPIDOutput / BUZZER_MAX_PWR) + "%");
    }
    if (humidityControlPID.GetMode() == AUTOMATIC) {
      logln("[PID] -> Humidifier output is: " + String (100 * humidityControlPIDOutput / humidifierTimeCycle) + "%");
    }
    if (airControlPID.GetMode() == AUTOMATIC || skinControlPID.GetMode() == AUTOMATIC || humidityControlPID.GetMode() == AUTOMATIC) {
    logln("[PID] -> Desired temp is: " + String (desiredControlTemperature) + "ºC");
    }

    logln("[SENSORS] -> System current consumption is: " + String (digitalCurrentSensor.getCurrent(INA3221_CH1),2) + " Amps");
    logln("[SENSORS] -> Baby temperature: " + String(temperature[skinSensor]) + "ºC, correction error is " + String(errorTemperature[skinSensor]));
    logln("[SENSORS] -> Air temperature: " + String(temperature[airSensor]) + "ºC, correction error is " + String(errorTemperature[airSensor]));
    logln("[SENSORS] -> Humidity: " + String(humidity) + "%");
    if (millis() - lastDebugUpdate) {
      logln("[LATENCY] -> Looped " + String(loopCounts * 1000 / (millis() - lastDebugUpdate)) + " Times per second");
    }
    loopCounts = 0;
    lastDebugUpdate = millis();
  }
  if (millis() - lastGraphicSensorsUpdate > sensorsUpdatePeriod) {
    updateRoomSensor();
    if (page == mainMenuPage || page == actuatorsProgressPage) {
      updateDisplaySensors();
    }
    lastGraphicSensorsUpdate = millis();
  }
  if ((page == mainMenuPage) && !enableSet) {
    checkSetMessage();
  }
}

void updateDisplaySensors() {
  float temperatureToUpdate;
  if (page == mainMenuPage || (page == actuatorsProgressPage)) {
    drawSelectedTemperature();
    drawHumidity();
  }
  if (page == actuatorsProgressPage) {
    drawUnselectedTemperature();
    setTextColor(COLOR_MENU_TEXT);
    if (controlTemperature) {
      float previousTemperaturePercentage = temperaturePercentage;
      if (displayProcessPercentage) {
        drawRightNumber(temperaturePercentage, tft.width() / 2, temperatureY);
      }
      if (controlMode) {
        temperatureToUpdate = temperature[airSensor];
      }
      else {
        temperatureToUpdate = temperature[skinSensor];
      }
      if ((desiredControlTemperature - temperatureAtStart)) {
        temperaturePercentage = 100 - ((desiredControlTemperature - temperatureToUpdate) * 100 / (desiredControlTemperature - temperatureAtStart));
      }
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
      if ((desiredControlHumidity - humidityAtStart)) {
        humidityPercentage = 100 - ((desiredControlHumidity - humidity) * 100 / (desiredControlHumidity - humidityAtStart));
      }
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
  Serial.print(dataString);
}

void GPRSLocalLog() {
  if (SDCard) {
    /*
      GPIOWrite(SD_CS, LOW);
      GPIOWrite(TFT_CS, HIGH);
      String dataString;
      dataString += temperature[skinSensor];
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

void backlightHandler() {
  if (autoLock) {
    if (millis() - lastbacklightHandler > time_lock) {
      if (ScreenBacklightMode != BL_POWERSAVE) {
        ledcWrite(SCREENBACKLIGHT_PWM_CHANNEL, backlightPowerSafe);
        ScreenBacklightMode = BL_POWERSAVE;
      }
    }
    else {
      if (ScreenBacklightMode != BL_NORMAL) {
        ledcWrite(SCREENBACKLIGHT_PWM_CHANNEL, backlightPower);
        ScreenBacklightMode = BL_NORMAL;
      }
    }
  }
  else {
    if (ScreenBacklightMode != BL_NORMAL) {
      ledcWrite(SCREENBACKLIGHT_PWM_CHANNEL, backlightPower);
      ScreenBacklightMode = BL_NORMAL;
    }
  }
}
