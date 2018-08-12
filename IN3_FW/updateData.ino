int updateData() {
  newPosition = myEncoderRead();
  move = oldPosition - newPosition;
  if (abs(move) > 1) {
    if (move > 0) {
      move = 1;
    }
    else {
      move = -1;
    }
  }
  lastEncoderPos[counter] = encoderpos[counter];
  oldPosition = newPosition;
  //PROBLEM WITH THIS FUNCTION: temperature measure time is not constant
  if (page == 0 || page == 1) {
    if (millis() - last_temp_update > (temp_update_rate * temperature_measured / temperature_fraction)) {
      for (int ntc = 0; ntc < numNTC; ntc++) {
        temperatureArray[ntc][temperature_measured] = analogRead(NTCpin[ntc]);
      }
      temperature_measured++;
    }
    if (millis() - last_temp_update > temp_update_rate) {
      updateSensors();
      if (page == 1) {
        printStatus();
      }
    }
  }
  if (!page && !enableSet) {
    checkSetMessage();
  }
  return move;
}

void updateSensors() {
  tft.setTextColor(COLOR_MENU);
  if (page == 0 || page == 1) {
    tft.drawFloat(temperature[cornerNTC], 1, temperatureX, temperatureY, 4);
  }
  if (updateHumidity()) {
    tft.setTextColor(COLOR_HEADING);
    drawCentreNumber(humidity, humidityX, humidityY);
    tft.setTextColor(COLOR_MENU);
    drawCentreNumber(humidity, humidityX, humidityY);
  }
  updateTemp(bothNTC);
  tft.setTextColor(COLOR_MENU_TEXT);
  if (page == 0 || page == 1) {
    tft.drawFloat(temperature[cornerNTC], 1, temperatureX, temperatureY, 4);
  }
  if (page == 1) {
    drawRightNumber(processPercentage, tft.width() / 2, temperatureY);
    float previousPercentage = processPercentage;
    processPercentage = 100 - ((desiredIn3Temp - temperature[cornerNTC]) * 100 / (desiredIn3Temp - temperatureAtStart));
    if (processPercentage > 99) {
      processPercentage = 100;
    }
    if (processPercentage < 0) {
      processPercentage = 0;
    }
    updateLoadingBar(int(previousPercentage), int(processPercentage));
  }
  temperature_measured = 0;
  last_temp_update = millis();
}

bool updateHumidity() {
  timer.pause();
  int newTemp = dht.getTemperature();
  int newHumidity = dht.getHumidity();
  if (newHumidity && newTemp) {
    temperature[dhtSensor] = newTemp;
    humidity = newHumidity;
    humidity += diffHumidity;
  }
  timer.refresh();
  timer.resume();
  return (newHumidity);
}

void updateTemp(byte sensor) {
  //Valores fijos del circuito
  float rAux = 10000.0;
  float vcc = 3.3;
  float beta = 3950.0;
  float temp0 = 298.0;
  float r0 = 10000.0;
  float temperatureMean;
  byte startSensor;
  byte endSensor;

  switch (sensor) {
    case cornerNTC:
      startSensor = cornerNTC;
      endSensor = cornerNTC;
      break;
    case heaterNTC:
      startSensor = heaterNTC;
      endSensor = heaterNTC;
      break;
    case bothNTC:
      startSensor = cornerNTC;
      endSensor = heaterNTC;
      break;
  }

  //Bloque de cálculo
  for (int ntc = startSensor; ntc < endSensor; ntc++) {
    //Variables usadas en el cálculo
    float vm = 0.0;
    float rntc = 0.0;
    if (!temperature_measured) {
      temperatureMean = analogRead(NTCpin[ntc]) + diffTemperature[cornerNTC];
    }
    else {
      temperatureMean = 0;
      for (int i = 0; i < temperature_measured; i++) {
        temperatureMean += temperatureArray[ntc][i];
      }
      temperatureMean /= temperature_measured;
    }
    vm = (vcc) * ( temperatureMean / 4098 );          //Calcular tensión en la entrada
    rntc = rAux / ((vcc / vm) - 1);                   //Calcular la resistencia de la NTC
    temperature[ntc] = beta / (log(rntc / r0) + (beta / temp0)) - 273; //Calcular la temperatura en Celsius
    temperature[ntc] += diffTemperature[ntc];
  }
}

void updateLoadingBar(float prev, float actual) {
  if (prev != actual) {
    float diff = (actual - prev) / 100;
    int color;
    float barX;
    int barY, barDiffWidth;
    barX = barPosX - (barWidth / 2) * (1 - prev / 50);
    barY = barPosY - barHeight / 2;
    barDiffWidth = barWidth * abs(diff) + 1;
    if (diff > 0) {
      color = COLOR_LOADING_BAR;
    }
    else {
      color = COLOR_MENU;
      barX -= barDiffWidth - 1;
    }
    tft.fillRect(barX, barY, barDiffWidth, barHeight, color);
    tft.setTextColor(COLOR_MENU);
    drawRightNumber(prev, tft.width() / 2, temperatureY);
    tft.setTextColor(COLOR_MENU_TEXT);
    drawRightNumber(actual, tft.width() / 2, temperatureY);
  }
}

void printStatus() {
  Serial.print(millis());
  Serial.print(";");
  Serial.print(fanSpeed);
  Serial.print(";");
  Serial.print(heaterLimitTemp);
  Serial.print(";");
  Serial.print(desiredIn3Temp);
  Serial.print(";");
  Serial.print(heaterLimitTemp);
  Serial.print(";");
  for (int i = 0; i < numTempSensors; i++) {
    Serial.print(temperature[i]);
    Serial.print(";");
  }
  Serial.print(humidity);
  Serial.print(";");
  Serial.println(interruptcounter);
}

