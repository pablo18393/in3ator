#define consumptionMeanSamples 1000

void sensorsISR() {
  measurenumNTC();
  measureConsumption();
}

float sampleConsumption() {
  float consumption = 0;
  for (int i = 0; i <= consumptionMeanSamples; i++) {
    delayMicroseconds(100);
    if (measureConsumption()) {
      consumption = currentConsumption;
      return consumption;
    }
  }
  return consumption;
}

void measureOffsetConsumption() {
  /*
    currentConsumtionStacker = 0;
    for (int i = 0; i < consumptionMeanSamples; i++) {
    delay(1);
    currentConsumtionStacker += analogRead(SYSTEM_SHUNT);
    }
    currentOffset = currentConsumtionStacker / consumptionMeanSamples;
    logln("[SENSORS] -> Offset current consumption is: " + String (currentOffset * correctionCurrentFactor) + " Amps, instant measure is " + String(currentOffset));
    currentConsumtionStacker = 0;
  */
}

bool measureConsumption() {
  currentConsumptionPos++;
  if (currentConsumptionPos >= consumptionMeanSamples) {
    currentConsumptionPos = 0;
    currentConsumtionStacker = ((currentConsumtionStacker / consumptionMeanSamples) - currentOffset) * correctionCurrentFactor;
    if (currentConsumtionStacker > 0) {
      currentConsumption = currentConsumtionStacker;
    }
    currentConsumtionStacker = 0;
    return true;
  }
  else {
    currentConsumtionStacker += analogRead(SYSTEM_SHUNT);
    return false;
  }
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

void readPulsioximeter() {
  pulsioximeterInterSamples[pulsioximeterCount] = analogRead(PULSIOXIMETER);
  pulsioximeterMean *= (pulsioximeterMultiplierFilterFactor - pulsioximeterRestFilterFactor) / pulsioximeterMultiplierFilterFactor;
  pulsioximeterMean +=  pulsioximeterInterSamples[pulsioximeterCount] * pulsioximeterRestFilterFactor / pulsioximeterMultiplierFilterFactor;
}

void calculatePulsioximeterValues() {
  int meanPulsioximeterSamples;
  int pulsioximeterInterMean;
  for (int i = 0; i < pulsioximeterRate; i++) {
    pulsioximeterInterMean += pulsioximeterInterSamples[i] - pulsioximeterMean;
  }
  pulsioximeterInterMean /= pulsioximeterRate;
  pulsioximeterSample[pulsioximeterCounter[pulsioximeterSampled]][pulsioximeterSampled] = pulsioximeterInterMean;

  if (pulsioximeterCounter[pulsioximeterSampled] == maxPulsioximeterSamples) {
    pulsioximeterCounter[pulsioximeterSampled] = 0;
  }
  else {
    pulsioximeterCounter[pulsioximeterSampled]++;
  }
}


bool measurenumNTC() {
  for (int i = 0; i < numNTC; i++) {
    temperatureArray[i][temperature_measured] = analogRead(NTC_PIN[i]);
  }
  temperature_measured++;
  if (temperature_measured == temperature_fraction) {
    updateTemp(numNTC);
    temperature_measured = 0;
    return true;
  }
  return false;
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
    case babyNTC:
      startSensor = babyNTC;
      endSensor = babyNTC;
      break;
    case airNTC:
      startSensor = airNTC;
      endSensor = airNTC;
      break;
    case numNTC:
      startSensor = babyNTC;
      endSensor = airNTC;
      break;
  }

  //Bloque de cálculo
  for (int i = startSensor; i <= endSensor; i++) {
    //Variables used in calculus
    float vm = 0.0;
    float rntc = 0.0;
    temperatureMean = 0;
    for (int j = 0; j < temperature_fraction; j++) {
      temperatureMean += temperatureArray[i][j];
    }
    temperatureMean /= temperature_fraction;
    vm = (vcc) * ( temperatureMean / maxADCvalue );          //Calcular tensión en la entrada
    rntc = rAux / ((vcc / vm) - 1);                   //Calcular la resistencia de la NTC
    temperature[i] = beta / (log(rntc / r0) + (beta / temp0)) - 273; //Calcular la temperatura en Celsius
    temperature[i] += diffTemperature[i];
  }
}

bool updateRoomSensor() {
  if (roomSensorPresent == 0) {
    mySHTC3.update();
    temperature[digitalTempSensor] = mySHTC3.toDegC(); //Add here measurement to temp array
    humidity = int(mySHTC3.toPercent()) + diffHumidity;
  }
}

void userInteraction() {
  if (autoLock) {
    if (millis() - lastUserInteraction > time_lock) {
      pwmWrite(SCREENBACKLIGHT, backlightPowerSafe);
    }
    else {
      pwmWrite(SCREENBACKLIGHT, backlightPower);
    }
  }
  else {
    pwmWrite(SCREENBACKLIGHT, backlightPower);
  }
}
