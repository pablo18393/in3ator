//PID VARIABLES
#define tempCTL 0
#define humidityCTL 1
#define numPID 2

double Kp[numPID] = {30000, 200}, Ki[numPID] = {300, 2} , Kd[numPID] = {3000, 20};
double PIDOutput;
double temperaturePIDInput;
double humidityPIDOutput;
int humidifierTimeCycle = 1500;
unsigned long windowStartTime;
PID temperaturePID(&temperaturePIDInput, &PIDOutput, &desiredSkinTemp, Kp[tempCTL], Ki[tempCTL], Kd[tempCTL], P_ON_E, DIRECT);
PID humidityPID(&humidity, &humidityPIDOutput, &desiredRoomHum, Kp[humidityCTL], Ki[humidityCTL], Kd[humidityCTL], P_ON_E, DIRECT);

void PIDHandler() {
  if (temperaturePID.GetMode() == AUTOMATIC) {
    if (controlMode) {
      temperaturePIDInput = temperature[airNTC];
    }
    else {
      temperaturePIDInput = temperature[babyNTC];
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
        GPIOWrite(HUMIDIFIER, LOW);
        humidifierStateChange = false;
      }
      humidifierState = false;
    }
    else {
      if (!humidifierState || humidifierStateChange) {
        GPIOWrite(HUMIDIFIER, HIGH);
        humidifierStateChange = false;
      }
      humidifierState = true;
    }
  }
}

void configPIDTimer(int freq) {
  //PID timer configuration:
  //PIDTimer.pause();
  //PIDTimer.setPeriod(freq); // in microseconds
  //PIDTimer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  //PIDTimer.setCompare(TIMER_CH1, 1);  // Interrupt 1 count after each update
  //PIDTimer.attachCompare1Interrupt(PIDISR);
  //PIDTimer.refresh();
  //PIDTimer.resume();
}

void startTemperaturePID() {
  temperaturePID.SetOutputLimits(heaterMaxPWM * heaterPowerMin / 100, heaterMaxPWM * heaterPowerMax / 100);
  temperaturePID.SetTunings(Kp[tempCTL], Ki[tempCTL], Kd[tempCTL]);
  temperaturePID.SetControllerDirection(DIRECT);
  temperaturePID.SetMode(AUTOMATIC);
}

void stopTemperaturePID() {
  temperaturePID.SetMode(MANUAL);
  ledcWrite(HEATER_PWM_CHANNEL, LOW);
}

void startHumidityPID() {
  humidifierStateChange = true;
  windowStartTime = millis();
  humidityPID.SetOutputLimits(humidifierTimeCycle * humidifierDutyCycleMin / 100, humidifierTimeCycle * humidifierDutyCycleMax / 100);
  humidityPID.SetMode(AUTOMATIC);
}

void stopHumidityPID() {
  humidityPID.SetMode(MANUAL);
  GPIOWrite(HUMIDIFIER, LOW);
}
