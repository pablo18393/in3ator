int updateData() {
  if (millis() - lastGraphicSensorsUpdate > sensorsUpdateRate) {
    updateHumidity();
    if (page == advancedModePage || page == actuatorsProgressPage) {
      updateDisplaySensors();
      if (page == actuatorsProgressPage) {
        printStatus();
      }
      lastGraphicSensorsUpdate = millis();
    }
  }
  if ((page == mainMenuPage || page == advancedModePage) && !enableSet) {
    checkSetMessage();
  }
}

void updateDisplaySensors() {
  if (page == advancedModePage || (page == actuatorsProgressPage && controlTemperature)) {
    drawBabyTemperature();
    drawHumidity();
  }
  if (page == actuatorsProgressPage) {
    tft.setTextColor(COLOR_MENU_TEXT);
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



void printStatus() {
  //log(millis() / 1000);
  //log(";");
  //log(desiredSkinTemp);
  //log(";");
  //log(maxHeaterTemp);
  //log(";");
  for (int i = 0; i < numTempSensors; i++) {
    //log(temperature[i]);
    //log(";");
  }
  //log(desiredHeaterTemp);
  //log(";");
  //log(humidity);
  //log(";");
  //log(desiredRoomHum);
  //log(";");
  //log(heaterPower);
  //log(";");
  //logln(String(PIDOutput[heaterNTC]));
}
