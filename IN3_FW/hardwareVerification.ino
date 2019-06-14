//hardware verification. 1 is a mounted hardware, 0 a not mounted.
#define hardwareComponents 12
byte errorHardwareCode[hardwareComponents];
#define HWPowerEn 1
#define HWHeater 1
#define HWGeneric 1
#define HWFAN_HP 1
#define HWFAN_LP 1
#define HWFAN_EXTRA 1
#define HWICT 1
#define HWSterilize 1
#define HWHumidifier 1
#define HWNTCHeater 1
#define HWNTCRoom 1
#define HWHUMSensor 1

//number assigned to hardware
#define HW_NUM_PowerEn 0         //hardware 1
#define HW_NUM_Heater 1          //hardware 2
#define HW_NUM_Generic 2         //hardware 3
#define HW_NUM_FAN_HP 3            //hardware 4
#define HW_NUM_FAN_LP 4            //hardware 5
#define HW_NUM_FAN_EXTRA 5            //hardware 6
#define HW_NUM_ICT 6             //hardware 7
#define HW_NUM_Sterilize 7       //hardware 8
#define HW_NUM_Humidifier 8      //hardware 9
#define HW_NUM_NTCHeater 9       //hardware 10
#define HW_NUM_NTCRoom 10         //hardware 11
#define HW_NUM_HUMSensor 11       //hardware 12


//hardware critical check. 2 is a critical non interchangable hardware, 1 is a critical interchangable hardware, 0 a not critical and interchangable hardware.
#define HW_CRIT_PowerEn 2
#define HW_CRIT_Heater 1
#define HW_CRIT_Generic 0
#define HW_CRIT_FAN_HP 0
#define HW_CRIT_FAN_LP 0
#define HW_CRIT_FAN_EXTRA 0
#define HW_CRIT_ICT 1
#define HW_CRIT_Sterilize 1
#define HW_CRIT_Humidifier 1
#define HW_CRIT_NTCHeater 2
#define HW_CRIT_NTCRoom 2
#define HW_CRIT_HUMSENSOR 2

#define shortcircuit 2
#define opencircuit 1

bool hardwareMounted[] = {HWPowerEn, HWHeater, HWGeneric, HWFAN_HP, HWFAN_LP, HWFAN_EXTRA, HWICT, HWSterilize, HWHumidifier, HWNTCHeater, HWNTCRoom, HWHUMSensor};
bool hardwareCritical[] = {HW_CRIT_PowerEn, HW_CRIT_Heater, HW_CRIT_Generic, HW_CRIT_FAN_HP, HW_CRIT_FAN_LP, HW_CRIT_FAN_EXTRA, HW_CRIT_ICT, HW_CRIT_Sterilize, HW_CRIT_Humidifier, HW_CRIT_NTCHeater, HW_CRIT_NTCRoom, HW_CRIT_HUMSENSOR};
byte hardwareVerificationPin[] = {POWER_EN, HEATER, GENERIC, FAN_HP, FAN_LP, FAN_EXTRA, ICT, STERILIZE, HUMIDIFIER};
byte hardwareVerificationPin_FB[] = {POWER_EN_FB, HEATER_FB, GENERIC_FB, FAN_HP_FB, FAN_LP_FB, FAN_EXTRA_FB, ICT_FB, STERILIZE_FB, HUMIDIFIER_FB};
char* errorComponent[] = {"Power enable MOSFET", "Heater", "Generic", "FAN_HP", "FAN_LP", "FAN_EXTRA", "Jaundice LED", "Sterilizer", "Humidifier", "Temperature sensor", "Temperature sensor", "Humidity sensor"};
bool testOK;
bool testCritical;
bool criticalError;


void hardwareVerification() {
  numSensors = numNTC + dhtSensor;
  testOK = 1;
  testCritical = 0;
  for (int i = 0; i < hardwareComponents; i++) {
    errorHardwareCode[i] = 0;
  }
  shortcircuitTest();
  sensorsTest();
  if (!testCritical) {
    openCircuitTest();
  }
  for (int i = 0; i < hardwareComponents; i++) {
    if (errorHardwareCode[i]) {
      testOK = 0;
      Serial.print("hardware error code: ");
      Serial.print(errorComponent[i]);
      if (hardwareCritical[i] && errorHardwareCode[i] == shortcircuit) {
        Serial.print(", critical");
      }
      Serial.println();
    }
  }
  if (testOK) {
    Serial.println("HARDWARE TEST OK");
  }
  else {
    for (int i = 0; i < hardwareComponents; i++) {
      Serial.print(errorHardwareCode[i]);
    }
    Serial.println();
    Serial.println("HARDWARE TEST FAIL");
    drawHardwareErrorMessage();
  }
}

void shortcircuitTest() {
  for (int i = 0; i < hardwareComponents - numSensors; i++) {
    if (i) {
      digitalWrite(POWER_EN, HIGH);
    }
    errorHardwareCode[i] = 0;
    if (!digitalRead(hardwareVerificationPin[i]) && hardwareMounted[i]) {
      errorHardwareCode[i] = shortcircuit;
      if (hardwareCritical[i]) {
        testCritical = 1;
      }
    }
  }
  digitalWrite(POWER_EN, LOW);
}

void sensorsTest() {
  //sensors verification

  if ((analogRead(THERMISTOR_HEATER) > 3200 || analogRead(THERMISTOR_HEATER) < 1200) && HWNTCHeater) {
    errorHardwareCode[HW_NUM_NTCHeater] = opencircuit;
  }
  if ((analogRead(THERMISTOR_ROOM) > 3200 || analogRead(THERMISTOR_ROOM) < 1200) && HWNTCRoom) {
    errorHardwareCode[HW_NUM_NTCRoom] = opencircuit;
  }
  Serial.println(readHumSensor());
  if (HWHUMSensor && !readHumSensor()) {
    errorHardwareCode[HW_NUM_HUMSensor] = opencircuit;
  }
}

void openCircuitTest() {
  //power verification
  for (int i = 0; i < hardwareComponents - numSensors; i++) {
    digitalWrite(hardwareVerificationPin[i], HIGH);
    delayMicroseconds(mosfet_switch_time);
    if (digitalRead(hardwareVerificationPin_FB[i]) && hardwareMounted[i]) {
      errorHardwareCode[i] += opencircuit;
    }
    digitalWrite(hardwareVerificationPin[i], LOW);
  }
}
