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

//PID VARIABLES
#define skinPID 0
#define airPID 1
#define humidityPID 2
#define numPID 3

double Kp[numPID] = {200, 200, 200}, Ki[numPID] = {1, 1, 2} , Kd[numPID] = {500, 500, 20};
double HeaterPIDOutput;
double skinControlPIDInput;
double airControlPIDInput;
double humidityControlPIDOutput;
int humidifierTimeCycle = 5000;
unsigned long windowStartTime;
PID airControlPID(&airControlPIDInput, &HeaterPIDOutput, &desiredControlTemperature, Kp[airPID], Ki[airPID], Kd[airPID], P_ON_E, DIRECT);
PID skinControlPID(&skinControlPIDInput, &HeaterPIDOutput, &desiredControlTemperature, Kp[skinPID], Ki[skinPID], Kd[skinPID], P_ON_E, DIRECT);
PID humidityControlPID(&humidity, &humidityControlPIDOutput, &desiredControlHumidity, Kp[humidityPID], Ki[humidityPID], Kd[humidityPID], P_ON_E, DIRECT);

void PIDInit() {
  airControlPID.SetMode(MANUAL);
  skinControlPID.SetMode(MANUAL);
  humidityControlPID.SetMode(MANUAL);
}

void PIDHandler() {
  if (airControlPID.GetMode() == AUTOMATIC) {
    airControlPIDInput = temperature[airSensor];
    airControlPID.Compute();
    ledcWrite(HEATER_PWM_CHANNEL, HeaterPIDOutput * ongoingCriticalAlarm());
  }
  if (skinControlPID.GetMode() == AUTOMATIC) {
    skinControlPIDInput = temperature[skinSensor];
    skinControlPID.Compute();
    ledcWrite(HEATER_PWM_CHANNEL, HeaterPIDOutput * ongoingCriticalAlarm());
  }
  if (humidityControlPID.GetMode() == AUTOMATIC) {
    humidityControlPID.Compute();
    if (millis() - windowStartTime > humidifierTimeCycle)
    { //time to shift the Relay Window
      windowStartTime += humidifierTimeCycle;
    }
    if (humidityControlPIDOutput < millis() - windowStartTime) {
      if (humidifierState || humidifierStateChange) {
        in3_hum.turn(OFF);
        humidifierStateChange = false;
      }
      humidifierState = false;
    }
    else {
      if (!humidifierState || humidifierStateChange) {
        in3_hum.turn(ON);
        humidifierStateChange = false;
      }
      humidifierState = true;
    }
  }
}

void startPID(byte var) {
  switch (var) {
    case airPID:
      airControlPID.SetOutputLimits(false, BUZZER_MAX_PWR);
      airControlPID.SetTunings(Kp[airPID], Ki[airPID], Kd[airPID]);
      airControlPID.SetControllerDirection(DIRECT);
      airControlPID.SetMode(AUTOMATIC);
      break;
    case skinPID:
      skinControlPID.SetOutputLimits(false, BUZZER_MAX_PWR);
      skinControlPID.SetTunings(Kp[skinPID], Ki[skinPID], Kd[skinPID]);
      skinControlPID.SetControllerDirection(DIRECT);
      skinControlPID.SetMode(AUTOMATIC);
      break;
    case humidityPID:
      humidifierStateChange = true;
      windowStartTime = millis();
      humidityControlPID.SetOutputLimits(humidifierTimeCycle * humidifierDutyCycleMin / 100, humidifierTimeCycle * humidifierDutyCycleMax / 100);
      humidityControlPID.SetMode(AUTOMATIC);
      break;
  }

}

void stopPID(byte var) {
  switch (var) {
    case airPID:
      airControlPID.SetMode(MANUAL);
      break;
    case skinPID:
      skinControlPID.SetMode(MANUAL);
      break;
    case humidityPID:
      humidityControlPID.SetMode(MANUAL);
  }

}
