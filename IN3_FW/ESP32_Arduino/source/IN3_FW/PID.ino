//PID VARIABLES
#define tempCTL 0
#define humidityCTL 1
#define numPID 2

double Kp[numPID] = {200, 200}, Ki[numPID] = {1, 2} , Kd[numPID] = {500, 20};
double PIDOutput;
double temperaturePIDInput;
double humidityPIDOutput;
int humidifierTimeCycle = 5000;
unsigned long windowStartTime;
PID temperaturePID(&temperaturePIDInput, &PIDOutput, &desiredSkinTemp, Kp[tempCTL], Ki[tempCTL], Kd[tempCTL], P_ON_E, DIRECT);
PID humidityPID(&humidity, &humidityPIDOutput, &desiredRoomHum, Kp[humidityCTL], Ki[humidityCTL], Kd[humidityCTL], P_ON_E, DIRECT);

void PIDHandler() {
  if (temperaturePID.GetMode() == AUTOMATIC) {
    if (controlMode) {
      temperaturePIDInput = temperature[airSensor];
    }
    else {
      temperaturePIDInput = temperature[babySensor];
    }
    temperaturePID.Compute();
    ledcWrite(HEATER_PWM_CHANNEL, PIDOutput);
  }
  if (humidityPID.GetMode() == AUTOMATIC) {
    humidityPID.Compute();
    if (millis() - windowStartTime > humidifierTimeCycle)
    { //time to shift the Relay Window
      windowStartTime += humidifierTimeCycle;
    }
    if (humidityPIDOutput < millis() - windowStartTime) {
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

void startTemperaturePID() {
  temperaturePID.SetOutputLimits(false, heaterMaxPWM);
  temperaturePID.SetTunings(Kp[tempCTL], Ki[tempCTL], Kd[tempCTL]);
  temperaturePID.SetControllerDirection(DIRECT);
  temperaturePID.SetMode(AUTOMATIC);
}

void startHumidityPID() {
  humidifierStateChange = true;
  windowStartTime = millis();
  humidityPID.SetOutputLimits(humidifierTimeCycle * humidifierDutyCycleMin / 100, humidifierTimeCycle * humidifierDutyCycleMax / 100);
  humidityPID.SetMode(AUTOMATIC);
}


void stopTemperaturePID() {
  temperaturePID.SetMode(MANUAL);
}

void stopHumidityPID() {
  humidityPID.SetMode(MANUAL);
}
