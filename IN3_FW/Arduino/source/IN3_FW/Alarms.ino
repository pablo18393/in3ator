#define powerAlertNotificationPeriod 1000
long lastPowerAlertNotification;

void powerAlert() {
  if (millis() - lastPowerAlertNotification > powerAlertNotificationPeriod) {
    lastPowerAlertNotification = millis();
    logln("[ALARM] -> maximum power exceeded");
  }
}
