
void sensorsISR() {
  if (float(micros() - lastNTCmeasurement) / 1000 > float(NTCMeasurementPeriod / temperature_filter)) {
    lastNTCmeasurement = micros();
    measurenumNTC();
  }
  if (millis() - lastCurrentUpdate > CurrentUpdatePeriod) {
    lastCurrentUpdate = millis();
    currentConsumption = measureMeanConsumption();
  }
  currentMonitor();
}

void measureOffsetConsumption() {

}

float currentMonitor() {
  if (millis() - lastCurrentMeasurement > CurrentMeasurementPeriod ) {
    if (instantCurrent_array_pos < current_filter) {
      instantCurrent_array_pos++;
    }
    else {
      instantCurrent_array_pos = 0;
    }
    instantCurrent[instantCurrent_array_pos] = measureInstantConsumption();
    lastCurrentMeasurement = millis();
  }
}

float measureConsumptionForTime (long testTimeout) {
  long ongoingTestTime = millis();
  float instantCurrent;
  float averageConsumption = 0;
  long measuredTimes = 0;
  while (millis() - ongoingTestTime < testTimeout) {
    instantCurrent = measureInstantConsumption();
    if (instantCurrent) {
      averageConsumption += instantCurrent;
      measuredTimes++;
    }
  }
  if (measuredTimes) {
    averageConsumption / measuredTimes;
    averageConsumption = ADCScale(averageConsumption);
    logln("[TEST] -> tested " + String(measuredTimes) + " times");
    logln("[TEST] -> Current " + String(averageConsumption));
    return ADCToAmp(averageConsumption);
  } else {
    return false;
  }
}

float ADCToAmp(float var) {
  return (var * CurrentToAmpFactor);
}

float ADCScale (float var) {
  return (map (var, 0, 3000, 150, 2450));
}

float measureInstantConsumption() {
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

float measureMeanConsumption() {
  float currentMean;
  if (DIGITAL_CURRENT_SENSOR) {
    if (PAC.UpdateCurrent()) {
      Serial.println("[SENSORS] -> Current sensing returned zero");
      return (PAC.Current / 1000); //Amperes
    }
  }
  if (ANALOG_CURRENT_SENSOR) {
    for (int i = 0; i < instantCurrent_array_pos; i++) {
      currentMean += instantCurrent[i];
      instantCurrent[i] = 0;
    }
    currentMean /= instantCurrent_array_pos;
    currentMean *= CurrentToAmpFactor;
    return (currentMean);
  }
}

bool measurenumNTC() {
  for (int i = false; i < numNTC; i++) {
    temperatureArray[i][temperature_array_pos] = analogRead(NTC_PIN[i]);
  }
  temperature_array_pos++;
  if (temperature_array_pos >= temperature_filter) {
    updateTemp(numNTC);
    temperature_array_pos = false;
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
