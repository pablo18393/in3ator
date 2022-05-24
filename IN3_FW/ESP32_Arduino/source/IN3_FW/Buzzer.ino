void buzzerHandler() {
  if (millis() - buzzerTime > buzzerToneTime && buzzerBeeps) {
    buzzerBeeps -= buzzerBuzzing;
    buzzerBuzzing = !buzzerBuzzing;
    ledcWrite(BUZZER_PWM_CHANNEL, BUZZER_MAX_PWM / 2 * buzzerBuzzing);
    buzzerTime = millis();
  }
}

void buzzerConstantTone (int freq) {
  logln("[BUZZER] -> BUZZER activated in constant Mode");
  ledcWrite(BUZZER_PWM_CHANNEL, BUZZER_MAX_PWM / 2);
}

void shutBuzzer () {
  //logln("[BUZZER] -> BUZZER was shutted");
  ledcWrite(BUZZER_PWM_CHANNEL, false);
}

void buzzerTone (int beepTimes, int timevTaskDelay, int freq) {
  logln("[BUZZER] -> BUZZER beep mode activated  " + String(beepTimes) + " times");
  buzzerBeeps += beepTimes;
  buzzerToneTime = timevTaskDelay;
}
