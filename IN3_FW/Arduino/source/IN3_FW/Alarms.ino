#define powerAlertNotificationPeriod 1000
long lastPowerAlertNotification;
bool powerAlert;

void powerAlertISR() {
  if (millis() - lastPowerAlertNotification > powerAlertNotificationPeriod) {
    lastPowerAlertNotification = millis();
    powerAlert=1;
  }
}
