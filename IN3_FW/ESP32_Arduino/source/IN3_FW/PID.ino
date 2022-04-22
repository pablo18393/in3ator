//PID VARIABLES
#define airPID 0
#define skinPID 1
#define humidityPID 2
#define numPID 3

double Kp[numPID] = {200, 200, 200}, Ki[numPID] = {1, 1, 2} , Kd[numPID] = {500, 500, 20};
double HeaterPIDOutput;
double skinControlPIDInput;
double airControlPIDInput;
double humidityControlPIDOutput;
int humidifierTimeCycle = 5000;
unsigned long windowStartTime;
PID airControlPID(&airControlPIDInput, &HeaterPIDOutput, &desiredControlTemp, Kp[airPID], Ki[airPID], Kd[airPID], P_ON_E, DIRECT);
PID skinControlPID(&skinControlPIDInput, &HeaterPIDOutput, &desiredControlTemp, Kp[skinPID], Ki[skinPID], Kd[skinPID], P_ON_E, DIRECT);
PID humidityControlPID(&humidity, &humidityControlPIDOutput, &desiredRoomHum, Kp[humidityPID], Ki[humidityPID], Kd[humidityPID], P_ON_E, DIRECT);

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
        if (HUMIDIFIER_MODE == HUMIDIFIER_PWM) {
          ledcWrite(HUMIDIFIER_PWM_CHANNEL, false);
        }
        else {
          GPIOWrite(HUMIDIFIER, LOW);
        }
        humidifierStateChange = false;
      }
      humidifierState = false;
    }
    else {
      if (!humidifierState || humidifierStateChange) {
        if (HUMIDIFIER_MODE == HUMIDIFIER_PWM) {
          ledcWrite(HUMIDIFIER_PWM_CHANNEL, HUMIDIFIER_DUTY_CYCLE);
        }
        else {
          GPIOWrite(HUMIDIFIER, HIGH);
        }
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
