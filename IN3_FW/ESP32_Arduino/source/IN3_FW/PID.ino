//PID VARIABLES
#define airPID 0
#define skinPID 1
#define humidityPID 2
#define numPID 3

double Kp[numPID] = {200, 200, 200}, Ki[numPID] = {1, 1, 2} , Kd[numPID] = {500, 500, 20};
double PIDOutput;
double skinControlPIDInput;
double airControlPIDInput;
double humidityControlPIDOutput;
int humidifierTimeCycle = 5000;
unsigned long windowStartTime;
PID airControlPID(&airControlPIDInput, &PIDOutput, &desiredControlTemp, Kp[skinPID], Ki[skinPID], Kd[skinPID], P_ON_E, DIRECT);
PID skinControlPID(&skinControlPIDInput, &PIDOutput, &desiredControlTemp, Kp[skinPID], Ki[skinPID], Kd[skinPID], P_ON_E, DIRECT);
PID humidityControlPID(&humidity, &humidityControlPIDOutput, &desiredRoomHum, Kp[humidityPID], Ki[humidityPID], Kd[humidityPID], P_ON_E, DIRECT);

void PIDHandler() {
  if (skinControlPID.GetMode() == AUTOMATIC) {
    skinControlPID.Compute();
    ledcWrite(HEATER_PWM_CHANNEL, PIDOutput);
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
      skinControlPIDInput = temperature[airSensor];
      skinControlPID.SetOutputLimits(false, heaterMaxPWM);
      skinControlPID.SetTunings(Kp[airPID], Ki[airPID], Kd[airPID]);
      skinControlPID.SetControllerDirection(DIRECT);
      skinControlPID.SetMode(AUTOMATIC);
      break;
    case skinPID:
      skinControlPIDInput = temperature[babySensor];
      skinControlPID.SetOutputLimits(false, heaterMaxPWM);
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
