//post content variables
#define defaultPost 0
#define jaundiceLEDON 1
#define jaundiceLEDOFF 2
#define actuatorsModeON 3
#define actuatorsModeOFF 4
#define pulseFound 5
#define pulseLost 6
#define standByMode 7
#define addLocation 8
#define removeLocation 9

#define standByGPRSPostPeriod 800
#define actuatingGPRSPostPeriod 120
#define jaundiceGPRSPostPeriod 600
#define GPRSRoutinePeriod 1 // in millis
#define GPRS_SHUT OFF
long lastGPRSRoutine;

String user = "admin@admin.com";
String password = "admin";
String server = "pub.scar.io";
#define RX_BUFFER_LENGTH 2048

String databaseLogin[] = {
  "POST /In3/public/api/v1/auth/login?c=1 HTTP/1.1\n",
  "Host: " + server + "\n",
  "Content-Type: application/x-www-form-urlencoded\n",
  "Content-Length: 46\n",
  "Connection: close\n",
  "\n",
  "{\"email\":\"" + user + "\",\"password\":\"" + password + "\"}\n"
};

String databasePost[] = {
  "POST /In3/public/api/v1/session?c=1 HTTP/1.1\n",
  "Host: " + server + "\n",
  "Content-Type: application/x-www-form-urlencoded\n",
  "Authorization: Bearer \n",
  "Content-Length: 46\n",
  "Connection: close\n\n",
  "\n",
  ""
};

int len = false;
long GPRSTimeOut = 60000; //in millisecs

struct GPRSstruct {
  bool enable;
  bool firstPost;
  bool postSN;
  bool postBabyTemp;
  bool postAirTemp;
  bool postBoardTemp1;
  bool postBoardTemp2;
  bool postBoardTemp3;
  bool postHumidity;
  bool postLongitud;
  bool postLatitud;
  bool postJaundicePower;
  bool postBPM;
  bool postIBI;
  bool postRPD;
  bool postRSSI;
  bool postCurrentConsumption;
  bool postComment;
  int sendPeriod;
  long lastSent;
  char buffer[RX_BUFFER_LENGTH];
  int charToRead;
  int bufferPos;
  int bufferWritePos;
  String latitud;
  String longitud;
  String localDayTime;
  String localHourTime;
  String comment;
  String RSSI;
  bool readLatitud;
  bool readLongitud;
  bool readLocalDayTime;
  bool readLocalHourTime;
  String token;
  String reply;
  long lastEvent;
  bool processSuccess;
  bool readToken;
  bool initVars;
  bool powerUp;
  bool connect;
  bool connectionStatus;
  bool timeOut;
  byte process;
  long processTime;
  long packetSentenceTime;
  bool post;
  bool location;
  byte postProcess;
  bool validToken;
};

int GPRSsequence = false;

struct GPRSstruct GPRS;

void GPRS_Handler() {
  if (GPRS.enable) {
    if (GPRS.powerUp) {
      GPRSPowerUp();
    }
    if (GPRS.location) {
      getLocation();
    }
    if (GPRS.connect) {
      GPRSStablishConnection();
    }
    if (GPRS.post) {
      if (GPRS.validToken) {
        GPRSPost();
      }
      else {
        GPRSLogin();
      }
    }
    readGPRSData();
    GPRSStatusHandler();
  }
}


void GPRSStatusHandler() {
  if (GPRS.process) {
    if (GPRS.powerUp || GPRS.connect || GPRS.post || GPRS.location) {
      if (millis() - GPRS.processTime > GPRSTimeOut) {
        logln("[GPRS] -> timeOut: " + String(GPRS.powerUp) + String(GPRS.connect) + String(GPRS.post) + String(GPRS.location) + String(GPRS.process));
        GPRS.timeOut = false;
        GPRS.process = false;
        GPRS.post = false;
        GPRS.connect = false;
        GPRS.location = false;
        GPRS.powerUp = true;
        GPRS.validToken = false;
        logln("[GPRS] -> powering module down...");
        Serial2.print("AT+CPOWD=1\n");
        GPRS.packetSentenceTime = millis();
        GPRS.processTime = millis();
      }
    }
  }
  if (!GPRS.powerUp && !GPRS.connect && !GPRS.post) {
    if (!GPRS.connectionStatus) {
      GPRS.powerUp = true;
    }
    if (millis() - GPRS.lastSent > GPRS.sendPeriod * 1000) {
      logln("[GPRS] -> Posting GPRS data...");
      GPRS.post = true;
    }
  }
  if (!GPRS.firstPost && GPRS.connectionStatus) {
    //GPRS.location = true;
  }
}

void getLocation() {
  switch (GPRS.process) {
    case 0:
      GPRS.processTime = millis();
      GPRS.latitud = "";
      GPRS.longitud = "";
      GPRS.localDayTime = "";
      GPRS.localHourTime = "";
      GPRS.processSuccess = true;
      Serial2.print("AT+CGATT=1\n");
      GPRS.process++;
      break;
    case 1:
      checkSerial("OK", "ERROR");
      break;
    case 2:
      Serial2.print("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\n");
      GPRS.process++;
      break;
    case 3:
      checkSerial("OK", "ERROR");
      break;
    case 4:
      Serial2.print("AT+SAPBR=3,1,\"APN\",\"TM\"\n");
      GPRS.process++;
      break;
    case 5:
      checkSerial("OK", "ERROR");
      break;
    case 6:
      Serial2.print("AT+SAPBR=1,1\n");
      GPRS.process++;
      break;
    case 7:
      checkSerial("OK", "ERROR");
      break;
    case 8:
      Serial2.print("AT+SAPBR=2,1\n");
      GPRS.process++;
      break;
    case 9:
      checkSerial("OK", "ERROR");
      break;
    case 10:
      Serial2.print("AT+CIPGSMLOC=1,1\n");
      GPRS.process++;
      break;
    case 11:
      checkSerial("OK", "ERROR");
      break;
    case 12:
      if (!GPRS.processSuccess) {
        GPRS.powerUp = true;
      }
      else {
        if (!GPRS.firstPost) {
          GPRS.firstPost = true;
          GPRSSetPostVariables(addLocation, "");
        }
        else {
          GPRSSetPostVariables(addLocation, "");
        }
        GPRS.post = true;
      }
      GPRS.process = false;
      GPRS.location = false;
      GPRS.readLongitud = false;
      GPRS.readLatitud = false;
      break;
  }
}

void GPRSPowerUp() {
  switch (GPRS.process) {
    case 0:
      if (millis() - GPRS.packetSentenceTime > 2000) {
        GPRS.processTime = millis();
        GPIOWrite(GPRS_PWRKEY, LOW);
        GPRS.processSuccess = true;
        GPRS.process++;
        GPRS.packetSentenceTime = millis();
        logln("[GPRS] -> powering up GPRS");
      }
      break;
    case 1:
      if (millis() - GPRS.packetSentenceTime > 2000) {
        GPIOWrite(GPRS_PWRKEY, HIGH);
        GPRS.packetSentenceTime = millis();
        GPRS.process++;
        logln("[GPRS] -> GPRS powered");
      }
      break;
    case 2:
      if (millis() - GPRS.packetSentenceTime > 7000) {
        logln("[GPRS] -> Sending AT command");
        Serial2.print("AT\n");
        GPRS.packetSentenceTime = millis();
      }
      checkSerial("OK", "ERROR");
      break;
    case 3:
      if (GPRS.processSuccess) {
        GPRS.powerUp = false;
        GPRS.connect = true;
        logln("[GPRS] -> Power up success");
      }
      GPRS.process = false;
      break;
  }
}

void GPRSStablishConnection() {
  switch (GPRS.process) {
    case 0:
      logln("[GPRS] -> Stablishing connection");
      GPRS.processTime = millis();
      GPRS.processSuccess = true;
      Serial2.write("AT+CFUN=1\n");
      GPRS.packetSentenceTime = millis();
      GPRS.process++;
      break;
    case 1:
      checkSerial("OK", "ERROR");
      break;
    case 2:
      if (millis() - GPRS.packetSentenceTime > 4000) {
        Serial2.write("AT+CPIN?\n");
        GPRS.packetSentenceTime = millis();
        GPRS.process++;
      }
      break;
    case 3:
      checkSerial("OK", "ERROR");
      break;
    case 4:
      if (millis() - GPRS.packetSentenceTime > 4000) {
        Serial2.write("AT+CSTT=\"TM\",\"\",\"\"\n\n");
        GPRS.process++;
      }
      break;
    case 5:
      checkSerial("OK", "ERROR");
      break;
    case 6:
      Serial2.write("AT+CIICR\n");
      GPRS.process++;
      break;
    case 7:
      checkSerial("OK", "ERROR");
      break;
    case 8:
      Serial2.write("AT+CIFSR\n");
      GPRS.process++;
      break;
    case 9:
      Serial2.write("AT\n");
      GPRS.process++;
      break;
    case 10:
      checkSerial("OK", "ERROR");
      break;
    case 11:
      Serial2.write("AT+CSQ\n");
      GPRS.process++;
      break;
    case 12:
      GPRS.RSSI = checkSerial(",", "ERROR");
      if (GPRS.RSSI.length()) {
        if (GPRS.RSSI.length() == 10) {
          GPRS.RSSI = String(GPRS.RSSI.charAt(7)) + String(GPRS.RSSI.charAt(8));
        }
        else {
          GPRS.RSSI = String(GPRS.RSSI.charAt(7));
        }
      }
      break;
    case 13:
      if (GPRS.processSuccess) {
        GPRS.connectionStatus = true;
      }
      else {
        GPRS.powerUp = true;
      }
      GPRS.connect = false;
      GPRS.process = false;
      GPRS.post = true;
      break;
  }
}

void GPRSLogin() {
  switch (GPRS.process) {
    case 0:
      GPRS.processSuccess = true;
      GPRS.lastSent = millis();
      GPRS.processTime = millis();
      if (GPRSLoadVariables()) {
        logln("[GPRS] -> Starting POST process");
        Serial2.print("AT+CIPSTART=\"TCP\",\"" + server + "\",80\n");
        GPRS.process++;
        GPRSLocalLog(); //log GPRS data in SD module
      }
      else {
        logln("[GPRS] -> Wrong post value");
      }
      break;
    case 1:
      checkSerial("CONNECT OK", "ERROR");
      break;
    case 2:
      len = false;
      for (int i = false; i < 7; i++) {
        len += databaseLogin[i].length();
      }
      Serial2.print("AT+CIPSEND=" + String(len - 1) + "\n");
      GPRS.process++;
      GPRS.packetSentenceTime = millis();
      break;
    case 3:
      if (millis() - GPRS.packetSentenceTime > 200) {
        Serial2.print(databaseLogin[GPRSsequence]);
        GPRSsequence++;
        if (GPRSsequence == 7) {
          GPRS.process++;
          GPRSsequence = false;
        }
        GPRS.packetSentenceTime = millis();
      }
      break;
    case 4:
      checkSerial("SEND OK", "\n\n");
      break;
    case 5:
      checkSerial("HTTP/1.1 200 OK", "\n\n");
      break;
    case 6:
      checkSerial("CLOSED", "\n\n");
      break;
    case 7:
      GPRS.validToken = true;
      GPRS.process = false;
      break;
  }
}

void GPRSPost() {
  switch (GPRS.process) {
    case 0:
      GPRS.processTime = millis();
      if (GPRSLoadVariables()) {
        GPRS.process++;
      }
      break;
    case 1:
      if (GPRSsequence == false) {
        GPRS.processSuccess = true;
        GPRS.lastSent = millis();
        GPRS.processTime = millis();
        GPRS.token.trim();
        databasePost[3] = "Authorization: Bearer " + GPRS.token + "\n";
        databasePost[4] = "Content-Length: " + String(databasePost[7].length()) + "\n";
        len = false;
      }
      len += databasePost[GPRSsequence].length();
      GPRSsequence++;
      if (GPRSsequence == 8) {
        Serial2.print("AT+CIPSTART=\"TCP\",\"" + server + "\",80\n");
        GPRS.process++;
        GPRSsequence = false;
      }
      GPRS.packetSentenceTime = millis();
      break;
    case 2:
      checkSerial("CONNECT OK", "ERROR");
      break;
    case 3:
      Serial2.print("AT+CIPSEND=" + String(len - 1) + "\n");
      GPRS.packetSentenceTime = millis();
      GPRS.process++;
      break;
    case 4:
      if (millis() - GPRS.packetSentenceTime > 200) {
        if (GPRSsequence == 3 || GPRSsequence == 7) {
          for (int i = false; i < databasePost[GPRSsequence].length(); i++) {
            Serial2.print(databasePost[GPRSsequence][i]);
          }
        }
        else {
          Serial2.print(databasePost[GPRSsequence]);
        }
        GPRSsequence++;
        if (GPRSsequence == 8) {
          GPRS.process++;
          GPRSsequence = false;
        }
        GPRS.packetSentenceTime = millis();
      }
      break;
    case 5:
      checkSerial("HTTP/1.1 200 OK", "status");
      break;
    case 6:
      if (GPRS_SHUT) {
        Serial2.print("AT+CIPSHUT\n");
      }
      GPRS.process++;
      break;
    case 7:
      if (GPRS_SHUT) {
        checkSerial("SHUT OK", "ERROR");
      }
      else {
        GPRS.process++;
      }
      break;
    case 8:
      logln("[GPRS] -> GPRS POST SUCCESS");
      GPRS.post = false;
      GPRS.process = false;
      GPRS.comment = "";
      GPRSSetPostVariables(removeLocation, "");
      break;
  }
}

void readGPRSData() {
  while (Serial2.available()) {
    GPRS.buffer[GPRS.bufferWritePos] = Serial2.read();
    log(String(GPRS.buffer[GPRS.bufferWritePos]));
    if (GPRS.post) {
      if (!GPRS.readToken) {
        if (GPRS.buffer[GPRS.bufferWritePos] == char('y')) {
          if (GPRS.buffer[GPRS.bufferWritePos - 1] == char('e')) {
            GPRS.readToken = true;
            GPRS.token = "\n";
            GPRS.token += "ey";
          }
        }
      }
      else {
        if (GPRS.buffer[GPRS.bufferWritePos] == '\n') {
          GPRS.readToken = false;
        }
        else {
          GPRS.token += GPRS.buffer[GPRS.bufferWritePos];
        }
      }
    }
    else if (GPRS.location) {
      if (GPRS.process == 11) {
        if (GPRS.buffer[GPRS.bufferWritePos] == char(',') ) {
          if (GPRS.buffer[GPRS.bufferWritePos - 2] != char('=')) {
            if (GPRS.longitud == "") {
              GPRS.readLongitud = true;
            }
            else if (GPRS.latitud == "") {
              GPRS.readLongitud = false;
              GPRS.readLatitud = true;
            }
            else if (GPRS.localDayTime == "") {
              GPRS.readLocalDayTime = true;
              GPRS.readLatitud = false;
            }
            else {
              GPRS.readLocalDayTime = false;
              GPRS.readLocalHourTime = true;
            }
          }
        }
        else {
          if (GPRS.readLatitud) {
            GPRS.latitud += GPRS.buffer[GPRS.bufferWritePos];
          }
          if (GPRS.readLongitud) {
            GPRS.longitud += GPRS.buffer[GPRS.bufferWritePos];
          }
          if (GPRS.readLocalDayTime) {
            GPRS.localDayTime += GPRS.buffer[GPRS.bufferWritePos];
          }
          if (GPRS.readLocalHourTime) {
            if (GPRS.buffer[GPRS.bufferWritePos] == '\n') {
              GPRS.readLocalHourTime = false;
              //setTime((int(GPRS.localHourTime.charAt(0)) - 48) * 10 + (int(GPRS.localHourTime.charAt(1)) - 48), (int(GPRS.localHourTime.charAt(3)) - 48) * 10 + (int(GPRS.localHourTime.charAt(4)) - 48), (int(GPRS.localHourTime.charAt(6)) - 48) * 10 + (int(GPRS.localHourTime.charAt(7)) - 48), (int(GPRS.localDayTime.charAt(8)) - 48) * 10 + (int(GPRS.localDayTime.charAt(9)) - 48), (int(GPRS.localDayTime.charAt(5)) - 48) * 10 + (int(GPRS.localDayTime.charAt(6)) - 48), 2000 + (int(GPRS.localDayTime.charAt(2)) - 48) * 10 + (int(GPRS.localDayTime.charAt(3)) - 48)); // setTime(hr,min,sec,day,month,yr); // Another way to set
              logln("[GPRS] -> Local time: " + GPRS.localDayTime + "," + GPRS.localHourTime);
            }
            else {
              GPRS.localHourTime += GPRS.buffer[GPRS.bufferWritePos];
            }
          }
        }
      }
    }
    GPRS.bufferWritePos++;
    if (GPRS.bufferWritePos >= RX_BUFFER_LENGTH) {
      GPRS.bufferWritePos = 0;
      logln("[GPRS] -> Buffer overflow");
    }
    GPRS.charToRead++;
  }
}

String checkSerial(String success, String error) {
  if (!GPRS.initVars) {
    GPRS.initVars = true;
    GPRS.reply = "";
  }

  if (GPRS.charToRead) {
    GPRS.reply += String(GPRS.buffer[GPRS.bufferPos]);
    if (GPRS.bufferPos > 1023) {
      GPRS.bufferPos = false;
    }
    if (GPRS.buffer[GPRS.bufferPos] == '\r') {
      //log(GPRS.reply);
      GPRS.reply = "";
    }
    if (GPRS.buffer[GPRS.bufferPos] == success.charAt(success.length() - 1)) {
      bool foundSuccessString = true;
      for (int i = false; i < success.length(); i++) {
        if (GPRS.buffer[GPRS.bufferPos - success.length() + i + 1] != success.charAt(i)) {
          foundSuccessString = false;
        }
      }
      if (foundSuccessString) {
        //log(GPRS.reply);
        GPRS.initVars = false;
        GPRS.process++;
        clearGPRSBuffer();
        return (GPRS.reply);
      }
    }
    if (GPRS.buffer[GPRS.bufferPos] == success.charAt(error.length() - 1)) {
      bool foundErrorString = true;
      for (int i = false; i < error.length(); i++) {
        if (GPRS.buffer[GPRS.bufferPos - error.length() + i + 1] != error.charAt(i)) {
          foundErrorString = false;
        }
      }
      if (foundErrorString) {
        logln("[GPRS] -> GPRS error: " + error);
        //log(GPRS.reply);
        GPRS.processSuccess = false;
        GPRS.initVars = false;
        GPRS.process++;
        clearGPRSBuffer();
      }
    }
    GPRS.charToRead--;
    GPRS.bufferPos++;
  }
  return "";
}

void clearGPRSBuffer() {
  GPRS.charToRead = false;
  GPRS.bufferPos = false;
  GPRS.bufferWritePos = false;
}

void GPRSSetPostVariables(byte postContent, String postComment) {
  if (postComment.length()) {
    GPRS.comment += postComment;
  }
  switch (postContent) {
    case defaultPost:
      GPRS.postBabyTemp = true;
      GPRS.postAirTemp = true;
      GPRS.postBoardTemp1 = false;
      GPRS.postBoardTemp2 = false;
      GPRS.postBoardTemp3 = true;
      GPRS.postHumidity = true;
      GPRS.RSSI = true;
      GPRS.postCurrentConsumption = true;
      GPRS.postComment = true;
      break;
    case addLocation:
      GPRS.postLatitud = false;
      GPRS.postLongitud = false;
      break;
    case removeLocation:
      GPRS.postLatitud = false;
      GPRS.postLongitud = false;
      break;
    case jaundiceLEDON:
      GPRS.postJaundicePower = true;
      break;
    case jaundiceLEDOFF:
      GPRS.postJaundicePower = false;
      break;
    case pulseFound:
      GPRS.postBPM = true;
      GPRS.postIBI = true;
      GPRS.postRPD = true;
      break;
    case pulseLost:
      GPRS.postBPM = false;
      GPRS.postIBI = false;
      GPRS.postRPD = false;
      break;
  }
}

void setGPRSPostPeriod (long seconds) {
  GPRS.sendPeriod = seconds;
}

bool GPRSLoadVariables() {
  databasePost[7] = "{";
  databasePost[7] += "\"sn\":\"" + serialNumber + "\"";
  if (GPRS.postBabyTemp) {
    databasePost[7] += ",\"baby_temp\":\"" + String(temperature[babyNTC], 2) + "\"";
  }
  if (GPRS.postAirTemp) {
    databasePost[7] += ",\"heater_temp\":\"" + String(temperature[airNTC], 2) + "\"";
  }
  if (GPRS.postBoardTemp1) {
    //databasePost[7] += ",\"board_temp1\":\"" + String(temperature[inBoardLeftNTC], 2) + "\"";
  }
  if (GPRS.postBoardTemp2) {
    //databasePost[7] += ",\"board_temp2\":\"" + String(temperature[inBoardRightNTC], 2) + "\"";
  }
  if (GPRS.postBoardTemp3) {
    databasePost[7] += ",\"board_temp3\":\"" + String(temperature[digitalTempSensor], 2) + "\"";
  }
  if (GPRS.postHumidity) {
    databasePost[7] += ",\"humidity\":\"" + String(humidity) + "\"";
  }
  if (GPRS.postLongitud) {
    databasePost[7] += ",\"long\":\"" + GPRS.longitud + "\"";
  }
  if (GPRS.postLatitud) {
    databasePost[7] += ",\"lat\":\"" + GPRS.latitud + "\"";
  }
  if (GPRS.RSSI) {
    databasePost[7] += ",\"rssi\":\"" + GPRS.RSSI + "\"";
  }
  if (GPRS.postJaundicePower) {
    databasePost[7] += ",\"jaundice_power\":\"" + String(jaundiceEnable) + "\"";
  }
  if (GPRS.postBPM) {
    databasePost[7] += ",\"bpm\":\"" + String(BPM) + "\"";
  }
  if (GPRS.postIBI) {
    databasePost[7] += ",\"ibi\":\"" + String(IBI) + "\"";
  }
  if (GPRS.postRPD) {
    databasePost[7] += ",\"rpd\":\"" + String(IBI) + "\"";
  }
  if (GPRS.postCurrentConsumption) {
    databasePost[7] += ",\"power\":\"" + String(currentConsumption) + "\"";
  }
  if (GPRS.postComment) {
    if (GPRS.comment.length() < 20) {
      GPRS.comment += "Baby Tmax/Tmin " + String (temperatureMax[babyNTC], 2) + "/" + String (temperatureMin[babyNTC], 2);
      GPRS.comment += ";Air Tmax/Tmin " + String (temperatureMax[airNTC], 2) + "/" + String (temperatureMin[airNTC], 2);
      GPRS.comment += "CM:" + String(controlMode);
      temperatureMax[babyNTC] = temperatureMaxReset;
      temperatureMin[babyNTC] = temperatureMinReset;
      temperatureMax[airNTC] = temperatureMaxReset;
      temperatureMin[airNTC] = temperatureMinReset;
    }
    databasePost[7] += ",\"comments\":\"" + GPRS.comment + "\"";
  }
  databasePost[7] += "}\n";
  logln("[GPRS] -> Posting: " + databasePost[7]);
  if (databasePost[7].indexOf("nan") >= 0) {
    logln("[GPRS] -> Detected zero");
    return false;
  }
  if (databasePost[7].length() > 1200) {
    logln("[GPRS] -> posting packet size oversized");
    return false;
  }
  return true;
}
