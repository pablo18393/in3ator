/*
  MIT License

  Copyright (c) 2022 Medical Open World, Pablo Sánchez Bergasa

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

*/

long lastHumToggle;
bool humToggle;

void updateData() {
  loopCounts++;
  buzzerHandler();
  watchdogReload();
  OTAHandler();
  sensorsHandler();
  GPRS_Handler();
  backlightHandler();
  if (page != autoCalibrationPage) {
    securityCheck();
  }
  if (powerAlert) {
    logln("[ALARM] -> maximum power exceeded");
    powerAlert = false;
  }
  if (encPulseDetected && digitalRead(ENC_SWITCH)) {
    encPulseDetected = false;
  }
  if (millis() - lastDebugUpdate > debugUpdatePeriod) {
    if (airControlPID.GetMode() == AUTOMATIC) {
      logln("[PID] -> Heater PWM output is: " + String (100 * HeaterPIDOutput / BUZZER_MAX_PWR) + "%");
    }
    if (skinControlPID.GetMode() == AUTOMATIC) {
      logln("[PID] -> Heater PWM output is: " + String (100 * HeaterPIDOutput / BUZZER_MAX_PWR) + "%");
    }
    if (humidityControlPID.GetMode() == AUTOMATIC) {
      logln("[PID] -> Humidifier output is: " + String (100 * humidityControlPIDOutput / humidifierTimeCycle) + "%");
    }
    if (airControlPID.GetMode() == AUTOMATIC || skinControlPID.GetMode() == AUTOMATIC || humidityControlPID.GetMode() == AUTOMATIC) {
      logln("[PID] -> Desired temp is: " + String (desiredControlTemperature) + "ºC");
    }

    logln("[SENSORS] -> System current consumption is: " + String (digitalCurrentSensor.getCurrent(INA3221_CH1), 4) + " Amps");
    logln("[SENSORS] -> Phototherapy current consumption is: " + String (digitalCurrentSensor.getCurrent(INA3221_CH2), 4) + " Amps");
    logln("[SENSORS] -> Fan current consumption is: " + String (digitalCurrentSensor.getCurrent(INA3221_CH3), 4) + " Amps");
    logln("[SENSORS] -> Baby temperature: " + String(temperature[skinSensor]) + "ºC, correction error is " + String(errorTemperature[skinSensor]));
    logln("[SENSORS] -> Air temperature: " + String(temperature[airSensor]) + "ºC, correction error is " + String(errorTemperature[airSensor]));
    logln("[SENSORS] -> Humidity: " + String(humidity) + "%");
    if (millis() - lastDebugUpdate) {
      logln("[LATENCY] -> Looped " + String(loopCounts * 1000 / (millis() - lastDebugUpdate)) + " Times per second");
    }
    loopCounts = 0;
    lastDebugUpdate = millis();
  }
  if (millis() - lastGraphicSensorsUpdate > sensorsUpdatePeriod) {
    updateRoomSensor();
    if (page == mainMenuPage || page == actuatorsProgressPage) {
      updateDisplaySensors();
    }
    lastGraphicSensorsUpdate = millis();
  }
  if ((page == mainMenuPage) && !enableSet) {
    checkSetMessage(page);
  }
}

void updateDisplaySensors() {
  float temperatureToUpdate;
  if (page == mainMenuPage || (page == actuatorsProgressPage)) {
    drawSelectedTemperature(temperature[controlMode], previousTemperature[controlMode]);
    previousTemperature[controlMode] = temperature[controlMode];
    drawHumidity(humidity, previousHumidity);
    previousHumidity = humidity;
  }
  if (page == actuatorsProgressPage) {
    drawUnselectedTemperature(temperature[!controlMode], previousTemperature[!controlMode]);
    previousTemperature[!controlMode] = temperature[!controlMode];
    setTextColor(COLOR_MENU_TEXT);
    if (controlTemperature) {
      float previousTemperaturePercentage = temperaturePercentage;
      if (controlMode) {
        temperatureToUpdate = temperature[airSensor];
      }
      else {
        temperatureToUpdate = temperature[skinSensor];
      }
      if ((desiredControlTemperature - temperatureAtStart)) {
        temperaturePercentage = 100 - ((desiredControlTemperature - temperatureToUpdate) * 100 / (desiredControlTemperature - temperatureAtStart));
      }
      if (temperaturePercentage > 99) {
        temperaturePercentage = 100;
      }
      if (temperaturePercentage < 0) {
        temperaturePercentage = false;
      }
      updateLoadingTemperatureBar(int(previousTemperaturePercentage), int(temperaturePercentage));
    }
    if (controlHumidity) {
      float previousHumidityPercentage = humidityPercentage;
      if ((desiredControlHumidity - humidityAtStart)) {
        humidityPercentage = 100 - ((desiredControlHumidity - humidity) * 100 / (desiredControlHumidity - humidityAtStart));
      }
      if (humidityPercentage > 99) {
        humidityPercentage = 100;
      }
      if (humidityPercentage < 0) {
        humidityPercentage = false;
      }
      updateLoadingHumidityBar(int(previousHumidityPercentage), int(humidityPercentage));
    }
  }
}

void logln(String dataString) {
  log(String(millis() / 1000) + ": " + dataString + '\r' + '\n');
}

void log(String dataString) {
  if (SDCard) {
  }
  Serial.print(dataString);
}

void GPRSLocalLog() {
  if (SDCard) {
  }
}

void backlightHandler() {
  if (autoLock) {
    if (millis() - lastbacklightHandler > time_lock) {
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
