
void sensorsHandler() {
  /*
    if (float(micros() - lastNTCmeasurement) / 1000 > float(NTCMeasurementPeriod / temperature_filter)) {
    lastNTCmeasurement = micros();
    measureNTCTemperature();
    }
  */
  measureNTCTemperature();
  currentMonitor();
}

void measureOffsetConsumption() {

}

float currentMonitor() {
  if (micros() - lastCurrentMeasurement > CurrentMeasurementPeriod ) {
    previousCurrent[0] = measureInstantConsumption(MAIN_SHUNT);
    currentConsumption =  butterworth2(instantCurrent[1], instantCurrent[2], previousCurrent[0] , previousCurrent[1], previousCurrent[2]);
    instantCurrent[0] = currentConsumption;
    for (int i = 1; i >= 0; i--) {
      instantCurrent[i + 1] = instantCurrent[i];
      previousCurrent[i + 1] = previousCurrent[i];
    }
    lastCurrentMeasurement = micros();
  }
}

float ADCToAmp(byte currentSensor, float var) {
  switch (currentSensor) {
    case MAIN_SHUNT:
      return (var * currentToAmpFactor_MAIN);
      break;
    case HUMIDIFIER_SHUNT:
      return (var * currentToAmpFactor_HUMIDIFIER);
      break;
  }
}


float ADCScale (float var) {
  return (map (var, 0, 3000, 150, 2450));
}

float measureInstantConsumption(byte currentSensor) {
  if (digitalCurrentSensorPresent) {
    PAC.UpdateCurrent();
    return (PAC.Current / 1000); //Amperes
  }
  else {
    switch (currentSensor) {
      case MAIN_SHUNT:
        return (ADCToAmp(currentSensor, ADCScale(analogRead(SYS_SHUNT))));
        break;
      case HUMIDIFIER_SHUNT:
        return (ADCToAmp(currentSensor, ADCScale(analogRead(USB_SHUNT))));
        break;
    }
  }
}

long ADCmeasurements;
long sensorMeasurements;

float measureMeanConsumption(byte currentSensor, int samples) {
  float currentMean = 0;
  if (digitalCurrentSensorPresent) {
    PAC.UpdateCurrent();
    return (PAC.Current / 1000); //Amperes
  }
  else {
    for (int i = 0; i < samples; i++) {
      currentMean += measureInstantConsumption(currentSensor);
    }
    currentMean /= samples;
    return (currentMean);
  }
  return (false);
}

bool measureNTCTemperature() {
  float analogMeasurement;
  unsigned long start_time = millis(); // Record the start time so we can timeout
  if (externalADCpresent) {
    if (millis() - lastexternalADCRead > externalADCReadPeriod) {
      lastexternalADCRead = millis();
      if (ADCConversionReady()) {
        ADCmeasurements++;
        analogMeasurement = readExternalADC() / 3300;
        mySensor.setInputMultiplexer(ADS122C04_MUX_AIN1_AVSS);
        mySensor.start(); // Start the conversion
      }
      return true;
    }
  }
  else {
    if (micros() - lastNTCmeasurement > NTCMeasurementPeriod ) {
      previousTemperature[0] = adcToCelsius(analogRead(NTC_PIN[babySensor]), maxADCvalue);
      temperature[babySensor] =  butterworth2(instantTemperature[1], instantTemperature[2], previousTemperature[0] , previousTemperature[1], previousTemperature[2]);
      instantTemperature[0] = temperature[babySensor];
      for (int i = 1; i >= 0; i--) {
        instantTemperature[i + 1] = instantTemperature[i];
        previousTemperature[i + 1] = previousTemperature[i];
      }
      errorTemperature[babySensor] = temperature[babySensor];
      if (RawTemperatureRange[babySensor]) {
        temperature[babySensor] = (((temperature[babySensor] - RawTemperatureLow[babySensor]) * ReferenceTemperatureRange) / RawTemperatureRange[babySensor]) + ReferenceTemperatureLow;
      }
      errorTemperature[babySensor] -= temperature[babySensor];
      if (temperature[babySensor] < 0) {
        temperature[babySensor] = 0;
      }
      if (temperature[babySensor] > temperatureMax[babySensor]) {
        temperatureMax[babySensor] = temperature[babySensor];
      }
      if (temperature[babySensor] < temperatureMin[babySensor]) {
        temperatureMin[babySensor] = temperature[babySensor];
      }
      lastNTCmeasurement = micros();
    }
    return true;
  }
  return false;
}

float adcToCelsius(float adcReading,int maxAdcReading) {
  //Valores fijos del circuito
  float rAux = 10000.0;
  float vcc = 3.3;
  float beta = 3950.0;
  float temp0 = 298.0;
  float r0 = 10000.0;
  //Bloque de cálculo
  //Variables used in calculus
  float vm = 0.0;
  float rntc = 0.0;
  vm = (vcc) * ( adcReading / maxAdcReading);          //Calcular tensión en la entrada
  rntc = rAux / ((vcc / vm) - 1);                   //Calcular la resistencia de la NTC
  return (beta / (log(rntc / r0) + (beta / temp0)) - 273); //Calcular la temperatura en Celsius
}

bool updateRoomSensor() {
  if (roomSensorPresent) {
    bool sensorState = mySHTC3.update();
    logln("[SENSORS] -> Updating room humidity: state is " + String(sensorState));
    if (!sensorState) {
      temperature[airSensor] =  mySHTC3.toDegC(); //Add here measurement to temp array
      humidity = mySHTC3.toPercent();
      if (temperature[airSensor] > temperatureMax[airSensor]) {
        temperatureMax[airSensor] = temperature[airSensor];
      }
      if (temperature[airSensor] < temperatureMin[airSensor]) {
        temperatureMin[airSensor] = temperature[airSensor];
      }
      return true;
    }
    else {
      initRoomSensor();
      return false;
    }
  }
  else {
    initRoomSensor();
    return false;
  }
  return false;
}
