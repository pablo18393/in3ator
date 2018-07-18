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
  if (millis() - last_temp_update > (temp_update_rate * temperature_measured / temperature_fraction)) {
    temperatureArray[temperature_measured] = analogRead(THERMISTOR_CORNER);
    temperature_measured++;
  }
  if (millis() - last_temp_update > temp_update_rate) {
    updateSensors();
  }
  return move;
}

void updateSensors() {
  Serial.println(page);
  tft.setTextColor(COLOR_HEADING);
  drawCentreNumber(humidity, humidityX, humidityY);
  tft.setTextColor(COLOR_MENU);
  tft.drawFloat(temperature, 1, temperatureX, temperatureY, 4);
  updateHumidity();
  drawCentreNumber(humidity, humidityX, humidityY);
  updateTemp();
  tft.setTextColor(COLOR_MENU_TEXT);
  tft.drawFloat(temperature, 1, temperatureX, temperatureY, 4);
  if (page) {
    drawRightNumber(processPercentage, tft.width() / 2, temperatureY);
    float previousPercentage = processPercentage;
    processPercentage = 100 - ((desiredTemp - temperature) * 100 / (desiredTemp - temperatureAtStart));
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

void updateHumidity() {
  timer.pause();
  humidity = dht.getHumidity();
  timer.refresh();
  timer.resume();
}

void updateTemp() {
  //Valores fijos del circuito
  float rAux = 10000.0;
  float vcc = 3.3;
  float beta = 3950.0;
  float temp0 = 298.0;
  float r0 = 10000.0;
  float temperatureMean;

  //Variables usadas en el cálculo
  float vm = 0.0;
  float rntc = 0.0;

  //Bloque de cálculo
  if (!temperature_measured) {
    temperatureMean = analogRead(THERMISTOR_CORNER);
  }
  else {
    temperatureMean = 0;
    for (int i = 0; i < temperature_measured; i++) {
      temperatureMean += temperatureArray[i];
    }
    temperatureMean /= temperature_measured;
  }
  vm = (vcc) * ( temperatureMean / 4098 );          //Calcular tensión en la entrada
  rntc = rAux / ((vcc / vm) - 1);                   //Calcular la resistencia de la NTC
  temperature = beta / (log(rntc / r0) + (beta / temp0)) - 273; //Calcular la temperatura en Celsius
  if (temperatureAtStart > temperature) {
    temperatureAtStart = temperature;
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
