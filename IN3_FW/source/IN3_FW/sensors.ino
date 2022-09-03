
void sensorsHandler() {
  measureNTCTemperature();
  //currentMonitor();
}

float measureMeanConsumption(int shunt) {
  if (digitalCurrentSensorPresent) {
    return (digitalCurrentSensor.getCurrent(ina3221_ch_t(shunt))); //Amperes
  }
  return (false);
}

bool measureNTCTemperature() {
  float analogMeasurement;
  unsigned long start_time = millis(); // Record the start time so we can timeout
  int NTCmeasurement;
  if (micros() - lastNTCmeasurement > NTCMeasurementPeriod ) {
    NTCmeasurement = analogRead(NTC_PIN[skinSensor]);
    if (NTCmeasurement > minimumAllowedNTCMeasurement && NTCmeasurement < maximumAllowedNTCMeasurement) {
      lastSuccesfullSensorUpdate[skinSensor] = millis();
      previousTemperature[0] = adcToCelsius(NTCmeasurement, maxADCvalue);
      temperature[skinSensor] =  butterworth2(instantTemperature[1], instantTemperature[2], previousTemperature[0] , previousTemperature[1], previousTemperature[2]);
      instantTemperature[0] = temperature[skinSensor];
      for (int i = 1; i >= 0; i--) {
        instantTemperature[i + 1] = instantTemperature[i];
        previousTemperature[i + 1] = previousTemperature[i];
      }
      errorTemperature[skinSensor] = temperature[skinSensor];
      if (RawTemperatureRange[skinSensor]) {
        temperature[skinSensor] = (((temperature[skinSensor] - RawTemperatureLow[skinSensor]) * ReferenceTemperatureRange) / RawTemperatureRange[skinSensor]) + ReferenceTemperatureLow;
        temperature[skinSensor] += fineTuneSkinTemperature;
      }
      errorTemperature[skinSensor] -= temperature[skinSensor];
      if (temperature[skinSensor] < 0) {
        temperature[skinSensor] = 0;
      }
      if (temperature[skinSensor] > temperatureMax[skinSensor]) {
        temperatureMax[skinSensor] = temperature[skinSensor];
      }
      if (temperature[skinSensor] < temperatureMin[skinSensor]) {
        temperatureMin[skinSensor] = temperature[skinSensor];
      }
      lastNTCmeasurement = micros();
    }
    return true;
  }
  return false;
}

float adcToCelsius(float adcReading, int maxAdcReading) {
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
  if (maxAdcReading && adcReading && (adcReading != maxAdcReading)) {
    vm = (vcc) * ( adcReading / maxAdcReading);          //Calcular tensión en la entrada
    rntc = rAux / ((vcc / vm) - 1);                   //Calcular la resistencia de la NTC
  }
  else {
    return false;
  }
  return (beta / (log(rntc / r0) + (beta / temp0)) - 273); //Calcular la temperatura en Celsius
}

bool updateRoomSensor() {
  if (roomSensorPresent) {
    noInterrupts();
    bool sensorState = mySHTC3.update();
    float sensedTemperature;
    logln("[SENSORS] -> Updating room humidity: state is " + String(sensorState));
    if (!sensorState) {
      sensedTemperature = mySHTC3.toDegC();
      if (sensedTemperature > minTempToDiscard && sensedTemperature < maxTempToDiscard) {
        lastSuccesfullSensorUpdate[airSensor] = millis();
        temperature[airSensor] =  sensedTemperature; //Add here measurement to temp array
        humidity = mySHTC3.toPercent();
        if (temperature[airSensor] > temperatureMax[airSensor]) {
          temperatureMax[airSensor] = temperature[airSensor];
        }
        if (temperature[airSensor] < temperatureMin[airSensor]) {
          temperatureMin[airSensor] = temperature[airSensor];
        }
        return true;
      }
    }
    else {
      initRoomSensor();
    }
    interrupts();
  }
  else {
    initRoomSensor();
  }
  return false;
}
