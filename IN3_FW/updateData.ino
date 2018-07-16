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
  if (millis() - last_temp_update > temp_update_rate) {
    last_temp_update = millis();
    updateTemperature();
    temperature_measured = 0;
  }
  //PROBLEM WITH THIS FUNCTION: temperature measure time is not constant
  if (millis() - last_temp_update > (temp_update_rate * temperature_measured / temperature_fraction)) {
    temperatureArray[temperature_measured] = analogRead(THERMISTOR1);
    temperature_measured++;
  }
  return move;
}

void updateTemperature() {
  tft.setTextColor(ILI9341_BLACK);
  tft.drawFloat(temperature, 1, temperatureX, temperatureY, 4);
  updateTemp();
  if (page && !lockPercentage) {
    drawRightNumber(processPercentage, tft.width() / 2, temperatureY);
  }
  tft.setTextColor(ILI9341_WHITE);
  tft.drawFloat(temperature, 1, temperatureX, temperatureY, 4);
  if (page && !lockPercentage) {
    Serial.println(desiredTemp);
    Serial.println(temperature);
    Serial.println(temperatureAtStart);
    processPercentage = 100 - ((desiredTemp - temperature) * 100 / (desiredTemp - temperatureAtStart));
    Serial.println(processPercentage);
    if (processPercentage > 99) {
      lockPercentage = 1;
      processPercentage = 100;
    }
    if (processPercentage < 0) {
      processPercentage = 0;
    }
    drawRightNumber(processPercentage, tft.width() / 2, temperatureY);
  }
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
  if (firstTemperatureMeasure) {
    firstTemperatureMeasure = 0;
    temperatureMean = analogRead(THERMISTOR1);
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
}


