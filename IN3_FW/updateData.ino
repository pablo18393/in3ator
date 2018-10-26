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
  checkSerialPort();
  return move;
}

void updateSensors() {
  tft.setTextColor(COLOR_MENU);
  if (page == menuPage || (page == actuatorsProgressPage && controlTemperature)) {
    tft.drawFloat(previousTemperature[roomNTC], 1, temperatureX, temperatureY, textFontSize);
  }
  if (page != actuatorsProgressPage || controlHumidity) {
    drawHumidity();
  }
  updateTemp(bothNTC);
  tft.setTextColor(COLOR_MENU_TEXT);
  if (page == menuPage || (page == actuatorsProgressPage && controlTemperature)) {
    tft.drawFloat(temperature[roomNTC], 1, temperatureX, temperatureY, textFontSize);
    previousTemperature[roomNTC] = temperature[roomNTC];
  }
  if (page == actuatorsProgressPage) {
    if (controlTemperature) {
      drawRightNumber(temperaturePercentage, tft.width() / 2, temperatureY);
      float previousTemperaturePercentage = temperaturePercentage;
      temperaturePercentage = 100 - ((desiredRoomTemp - temperature[roomNTC]) * 100 / (desiredRoomTemp - temperatureAtStart));
      if (temperaturePercentage > 99) {
        temperaturePercentage = 100;
      }
      if (temperaturePercentage < 0) {
        temperaturePercentage = 0;
      }
      updateLoadingTemperatureBar(int(previousTemperaturePercentage), int(temperaturePercentage));
    }
    if (controlHumidity) {
      drawRightNumber(humidityPercentage, tft.width() / 2, humidityY);
      float previousHumidityPercentage = humidityPercentage;
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

bool readHumSensor() {
  timer.pause();
  int newTemp = dht.getTemperature();
  int newHumidity = dht.getHumidity();
  timer.refresh();
  timer.resume();
  if (newHumidity && newTemp) {
    temperature[numNTC + dhtSensor] = newTemp;
    humidity = newHumidity;
    humidity += diffHumidity;
    return (1);
  }
  return (0);
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
    case roomNTC:
      startSensor = roomNTC;
      endSensor = roomNTC;
      break;
    case heaterNTC:
      startSensor = heaterNTC;
      endSensor = heaterNTC;
      break;
    case bothNTC:
      startSensor = roomNTC;
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
    tft.setTextColor(COLOR_MENU);
    drawRightNumber(prev, tft.width() / 2, temperatureY);
    tft.setTextColor(COLOR_MENU_TEXT);
    drawRightNumber(actual, tft.width() / 2, temperatureY);
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
    tft.setTextColor(COLOR_MENU);
    drawRightNumber(prev, tft.width() / 2, humidityY);
    tft.setTextColor(COLOR_MENU_TEXT);
    drawRightNumber(actual, tft.width() / 2, humidityY);
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

void turnActuatorsOff() {
  digitalWrite(HEATER, LOW);
  analogWrite(HEATER, 0);
  digitalWrite(HUMIDIFIER, LOW);
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
void checkSerialPort() {
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
          case 'P':
            Serial.println("ping");
            break;
          case 'R':
            nvic_sys_reset();
            break;
        }
        break;
      case 'H':
        diffHumidity = humidity - readSerialData();
        humidity -= diffHumidity;
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
  Serial.print(desiredRoomTemp);
  Serial.print(";");
  Serial.print(heaterLimitTemp);
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
  Serial.println(PIDOutput[heaterNTC]);
}

