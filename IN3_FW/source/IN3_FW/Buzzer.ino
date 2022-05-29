#define BUZZER_DISABLED false
#define BUZZER_ENABLED true

bool buzzerState;
int buzzerBeeps, buzzerToneTime;
long buzzerTime;
bool buzzerBuzzing;

void buzzerHandler() {
  if (millis() - buzzerTime > buzzerToneTime && buzzerBeeps) {
    buzzerBeeps -= buzzerBuzzing;
    buzzerBuzzing = !buzzerBuzzing;
    ledcWrite(BUZZER_PWM_CHANNEL, BUZZER_MAX_PWM / 2 * buzzerBuzzing);
    buzzerTime = millis();

    if (buzzerBeeps) {
      buzzerState = BUZZER_ENABLED;
    }
    else {
      buzzerState = BUZZER_DISABLED;
    }
  }
}

void buzzerConstantTone (int freq) {
  logln("[BUZZER] -> BUZZER activated in constant Mode");
  if (buzzerState == BUZZER_DISABLED) {
    ledcWrite(BUZZER_PWM_CHANNEL, BUZZER_MAX_PWM / 2);
    buzzerState = BUZZER_ENABLED;
  }
}

void shutBuzzer () {
  if (buzzerState == BUZZER_ENABLED) {
    logln("[BUZZER] -> BUZZER was shutted");
    ledcWrite(BUZZER_PWM_CHANNEL, false);
  }
}

void buzzerTone (int beepTimes, int timevTaskDelay, int freq) {
  logln("[BUZZER] -> BUZZER beep mode activated  " + String(beepTimes) + " times");
  buzzerBeeps += beepTimes;
  buzzerToneTime = timevTaskDelay;
}
