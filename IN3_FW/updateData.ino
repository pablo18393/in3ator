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
  if (page == menuPage || page == actuatorsProgressPage) {
    if (millis() - last_temp_update > temp_update_rate) {
      updateSensors();
      if (page == actuatorsProgressPage) {
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
  if (page == menuPage || page == actuatorsProgressPage) {
    tft.drawFloat(previousTemperature[cornerNTC], 1, temperatureX, temperatureY, textFontSize);
  }
  drawHumidity();
  updateTemp(bothNTC);
  tft.setTextColor(COLOR_MENU_TEXT);
  if (page == menuPage || page == actuatorsProgressPage) {
    tft.drawFloat(temperature[cornerNTC], 1, temperatureX, temperatureY, textFontSize);
    previousTemperature[cornerNTC] = temperature[cornerNTC];
  }
  if (page == actuatorsProgressPage) {
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
  last_temp_update = millis();
    Serial.println(temperatureY);
}

void drawHumidity() {
  int previousHumidity;
  if (updateHumidity()) {
    tft.setTextColor(COLOR_HEADING);
    drawCentreNumber(previousHumidity, humidityX, temperatureY);
    tft.setTextColor(COLOR_MENU);
    drawCentreNumber(humidity, humidityX, temperatureY);
    humidity = previousHumidity;
  }
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
  for (int ntc = startSensor; ntc <= endSensor; ntc++) {
    //Variables usadas en el cálculo
    float vm = 0.0;
    float rntc = 0.0;
    temperatureMean = 0;
    for (int i = 0; i < temperature_fraction; i++) {
      temperatureMean += temperatureArray[ntc][i];
    }
    temperatureMean /= temperature_fraction;
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

void measureAllNTC() {
  for (int ntc = 0; ntc < numNTC; ntc++) {
    temperatureArray[ntc][temperature_measured] = analogRead(NTCpin[ntc]);
  }
  temperature_measured++;
  if (temperature_measured == temperature_fraction) {
    temperature_measured = 0;
  }
}

void turnFansOn() {
  digitalWrite(FAN1, HIGH);
  digitalWrite(FAN2, HIGH);
  digitalWrite(FAN3, HIGH);
}

void turnFansOff() {
  digitalWrite(FAN1, LOW);
  digitalWrite(FAN2, LOW);
  digitalWrite(FAN3, LOW);
}

void printStatus() {
  Serial.print(millis());
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
  Serial.print(desiredHeaterTemp);
  Serial.print(";");
  Serial.println(PIDOutput[heaterNTC]);
}

