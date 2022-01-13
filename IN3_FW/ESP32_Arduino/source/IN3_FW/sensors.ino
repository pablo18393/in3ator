
void sensorsHandler() {
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
    instantCurrent[instantCurrent_array_pos] = measureInstantConsumption();
    instantCurrent_array_pos++;
    if (instantCurrent_array_pos >= current_filter) {
      instantCurrent_array_pos = 0;
    }
    lastCurrentMeasurement = millis();
  }
}

float measureConsumptionForTime (long testTimeout) {
  long ongoingTestTime = millis();
  float instant_Current;
  float averageConsumption = 0;
  long measuredTimes = 0;
  while (millis() - ongoingTestTime < testTimeout) {
    instant_Current = measureInstantConsumption();
    if (instant_Current) {
      averageConsumption += instant_Current;
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
  if (digitalCurrentSensorPresent) {
    PAC.UpdateCurrent();
    return (PAC.Current / 1000); //Amperes
  }
  else {
    return (analogRead(SYSTEM_SHUNT));
  }
}

long ADCmeasurements;
long sensorMeasurements;


float measureMeanConsumption() {
  float currentMean;
  if (digitalCurrentSensorPresent) {
    if (millis() - lastDigitalCurrentSensorRead > digitalCurrentSensorReadPeriod) {
      lastDigitalCurrentSensorRead = millis();
      sensorMeasurements++;
      PAC.UpdateCurrent();
      return (PAC.Current / 1000); //Amperes
    }
  }
  else {
    for (int i = 0; i < instantCurrent_array_pos; i++) {
      currentMean += instantCurrent[i];
      instantCurrent[i] = 0;
    }
    currentMean /= instantCurrent_array_pos;
    currentMean *= CurrentToAmpFactor;
    return (currentMean);
  }
  return (false);
}
bool measurenumNTC() {
  float analogMeasurement;
  unsigned long start_time = millis(); // Record the start time so we can timeout
  if (externalADCpresent) {
    if (millis() - lastexternalADCRead > externalADCReadPeriod) {
      lastexternalADCRead = millis();
      if (ADCConversionReady()) {
        ADCmeasurements++;
        analogMeasurement = readExternalADC() / 3300;
        switch (mySensor.getInputMultiplexer()) {
          case ADS122C04_MUX_AIN0_AVSS:
            temperatureArray[babyNTC][temperature_array_pos] = analogMeasurement;
            mySensor.setInputMultiplexer(ADS122C04_MUX_AIN1_AVSS);
            break;
          case ADS122C04_MUX_AIN1_AVSS:
            temperatureArray[airNTC][temperature_array_pos] = analogMeasurement;
            mySensor.setInputMultiplexer(ADS122C04_MUX_AIN0_AVSS);
            temperature_array_pos++;
            break;
          default:
            mySensor.setInputMultiplexer(ADS122C04_MUX_AIN1_AVSS);
            break;
        }
        mySensor.start(); // Start the conversion
      }
    }
  }
  else {
    for (int i = false; i < numNTC; i++) {
      analogMeasurement = analogRead(NTC_PIN[i]);
      temperatureArray[i][temperature_array_pos] = analogMeasurement;
    }
    temperature_array_pos++;
  }
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
    errorTemperature[i] = temperature[i];
    if (RawTemperatureRange[i]) {
      temperature[i] = (((temperature[i] - RawTemperatureLow[i]) * ReferenceTemperatureRange) / RawTemperatureRange[i]) + ReferenceTemperatureLow;
    }
    errorTemperature[i] -= temperature[i];

    if (temperature[i] > temperatureMax[i]) {
      temperatureMax[i] = temperature[i];
    }
    if (temperature[i] < temperatureMin[i]) {
      temperatureMin[i] = temperature[i];
    }
  }
}

bool updateRoomSensor() {
  logln("[SENSORS] -> Updating room humidity");
  if (roomSensorPresent) {
    mySHTC3.update();
    temperature[digitalTempSensor] =  mySHTC3.toDegC(); //Add here measurement to temp array
    if (temperature[digitalTempSensor]) {
      humidity = mySHTC3.toPercent();
      return true;
    }
    else {
      return false;
    }
  }
}
