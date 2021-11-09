#define consumptionMeanSamples 1000

void sensorsISR() {
  if (float(micros() - lastNTCmeasurement) / 1000 > float(NTCMeasurementPeriod / temperature_filter)) {
    lastNTCmeasurement = micros();
    measurenumNTC();
  }
  if (millis() - lastCurrentMeasurement > CurrentMeasurementPeriod) {
    lastCurrentMeasurement = millis();
    currentConsumption = measureConsumption();
  }
}

void measureOffsetConsumption() {

}

float measureConsumptionForTime (long testTimeout) {
  long ongoingTestTime = millis();
  float instantCurrent;
  float averageConsumption = 0;
  long measuredTimes = 0;
  while (millis() - ongoingTestTime < testTimeout) {
    instantCurrent = measureConsumption();
    if (instantCurrent) {
      averageConsumption += instantCurrent;
      measuredTimes++;
    }
  }
  if (measuredTimes) {
    return averageConsumption / measuredTimes;
  } else {
    return false;
  }
}

float measureConsumption() {
  if (DIGITAL_CURRENT_SENSOR) {
    if (PAC.UpdateCurrent()) {
      Serial.println("[SENSORS] -> Current sensing returned zero");
      return (PAC.Current / 1000); //Amperes
    }
  }
  if (ANALOG_CURRENT_SENSOR) {
    return (analogRead(SYSTEM_SHUNT));
  }
}

void checkNewPulsioximeterData() {
  if (pulsioximeterCounter[pulsioximeterDrawn] = !pulsioximeterCounter[pulsioximeterSampled]) {
    drawPulsioximeter();
    if (pulsioximeterCounter[pulsioximeterDrawn] == maxPulsioximeterSamples) {
      pulsioximeterCounter[pulsioximeterDrawn] = false;
    }
    else {
      pulsioximeterCounter[pulsioximeterDrawn]++;
    }
  }
}

void readPulsioximeter() {
  //pulsioximeterInterSamples[pulsioximeterCount] = analogRead(PULSIOXIMETER);
  pulsioximeterMean *= (pulsioximeterMultiplierFilterFactor - pulsioximeterRestFilterFactor) / pulsioximeterMultiplierFilterFactor;
  pulsioximeterMean +=  pulsioximeterInterSamples[pulsioximeterCount] * pulsioximeterRestFilterFactor / pulsioximeterMultiplierFilterFactor;
}

void calculatePulsioximeterValues() {
  int meanPulsioximeterSamples;
  int pulsioximeterInterMean;
  for (int i = false; i < pulsioximeterRate; i++) {
    pulsioximeterInterMean += pulsioximeterInterSamples[i] - pulsioximeterMean;
  }
  pulsioximeterInterMean /= pulsioximeterRate;
  pulsioximeterSample[pulsioximeterCounter[pulsioximeterSampled]][pulsioximeterSampled] = pulsioximeterInterMean;

  if (pulsioximeterCounter[pulsioximeterSampled] == maxPulsioximeterSamples) {
    pulsioximeterCounter[pulsioximeterSampled] = false;
  }
  else {
    pulsioximeterCounter[pulsioximeterSampled]++;
  }
}


bool measurenumNTC() {
  for (int i = false; i < numNTC; i++) {
    temperatureArray[i][temperature_measured] = analogRead(NTC_PIN[i]);
  }
  temperature_measured++;
  if (temperature_measured >= temperature_filter) {
    updateTemp(numNTC);
    temperature_measured = false;
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
    temperatureMean = false;
    for (int j = false; j < temperature_filter; j++) {
      temperatureMean += temperatureArray[i][j];
    }
    temperatureMean /= temperature_filter;
    vm = (vcc) * ( temperatureMean / maxADCvalue );          //Calcular tensión en la entrada
    rntc = rAux / ((vcc / vm) - 1);                   //Calcular la resistencia de la NTC
    temperature[i] = beta / (log(rntc / r0) + (beta / temp0)) - 273; //Calcular la temperatura en Celsius
    temperature[i] += diffTemperature[i];
    if (temperature[i] > temperatureMax[i]) {
      temperatureMax[i] = temperature[i];
    }
    if (temperature[i] < temperatureMin[i]) {
      temperatureMin[i] = temperature[i];
    }
  }
}

bool updateRoomSensor() {
  if (roomSensorPresent == true) {
    mySHTC3.update();
    temperature[digitalTempSensor] =  mySHTC3.toDegC(); //Add here measurement to temp array
    humidity = int(mySHTC3.toPercent()) + diffHumidity;
    if (temperature[digitalTempSensor]) {
      return true;
    }
    else {
      return false;
    }
  }
}

void userInteraction() {
  if (autoLock) {
    if (millis() - lastUserInteraction > time_lock) {
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
