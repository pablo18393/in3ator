// buffer de 768 para el serial

String user = "admin@admin.com";
String password = "admin";
String server = "pub.scar.io";
char GSMRXBuffer[1024];

String req[] = {
  "POST /In3/public/api/v1/auth/login?c=1 HTTP/1.1\n",
  "Host: " + server + "\n",
  "Content-Type: application/x-www-form-urlencoded\n",
  "Content-Length: 46\n",
  "Connection: close\n",
  "\n",
  "{\"email\":\"" + user + "\",\"password\":\"" + password + "\"}\n"
};

String req2[] = {
  "POST /In3/public/api/v1/session?c=1 HTTP/1.1\n",
  "Host: " + server + "\n",
  "Content-Type: application/x-www-form-urlencoded\n",
  "Authorization: Bearer \n",
  "Content-Length: 46\n",
  "Connection: close\n\n",
  "\n",
  ""
};

int len = 0;
long GSMTimeOut = 30; //in secs
struct GSMstruct {
  bool firstPost;
  bool error;
  bool postSN;
  bool postBabyTemp;
  bool postHeaterTemp;
  bool postBoardTemp1;
  bool postBoardTemp2;
  bool postBoardTemp3;
  bool postHumidity;
  bool postLongitud;
  bool postLatitud;
  bool postJaundicePower;
  bool postBPM;
  bool postIBI;
  bool postRPS;
  bool postHeaterPower;
  int sendPeriod;
  long lastSent;
  char buffer[1024];
  int charToRead;
  int bufferPos;
  int bufferWritePos;
  String latitud;
  String longitud;
  String localDayTime;
  String localHourTime;
  bool readLatitud;
  bool readLongitud;
  bool readLocalDayTime;
  bool readLocalHourTime;
  String token;
  String line;
  String lastLine;
  long lastEvent;
  bool processSuccess;
  bool readToken;
  bool enable;
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
};

int GSMsequence = 0;

struct GSMstruct GSM;

void initGPRS()
{
  Serial.begin(115200);
  Serial1.begin(115200);
  GSM.sendPeriod = 180; //in secs
  GSM.postBabyTemp = 1;
  GSM.postHumidity = 1;
  GSM.powerUp = 1;
}

void GSMHandler() {
  if (GSM.powerUp) {
    GSMPowerUp();
  }
  if (GSM.connect) {
    GSMStablishConnection();
  }
  if (GSM.location) {
    GSMLocation();
  }
  if (GSM.post) {
    GSMPost();
  }
  readGSMData();
  checkFirstPost();
  GSMStatusHandler();
}

void GSMStatusHandler() {
  if (GSM.timeOut) {
    GSM.timeOut = 0;
    GSM.process = 0;
    GSM.post = 0;
    GSM.connect = 0;
    GSM.powerUp = 1;
  }
  if (GSM.powerUp || GSM.connect || GSM.post || GSM.location) {
    if (millis() - GSM.processTime > GSMTimeOut * 1000) {
      GSM.timeOut = 1;
      logln("timeOut" + String(GSM.powerUp) + String(GSM.connect) + String(GSM.post) + String(GSM.location) + String(GSM.process));
      GSM.processTime = millis();
    }
  }

  if (!GSM.powerUp && !GSM.connect && !GSM.post) {
    if (!GSM.connectionStatus) {
      GSM.powerUp = 1;
    }
    if (millis() - GSM.lastSent > GSM.sendPeriod * 1000) {
      GSM.post = 1;
    }
  }
}

void GSMLocation() {
  switch (GSM.process) {
    case 0:
      GSM.processTime = millis();
      GSM.latitud = "";
      GSM.longitud = "";
      GSM.localDayTime = "";
      GSM.localHourTime = "";
      GSM.processSuccess = 1;
      Serial1.print("AT+CGATT=1\n");
      GSM.process++;
      break;
    case 1:
      checkSerial("OK", "ERROR");
      break;
    case 2:
      Serial1.print("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\n");
      GSM.process++;
      break;
    case 3:
      checkSerial("OK", "ERROR");
      break;
    case 4:
      Serial1.print("AT+SAPBR=3,1,\"APN\",\"TM\"\n");
      GSM.process++;
      break;
    case 5:
      checkSerial("OK", "ERROR");
      break;
    case 6:
      Serial1.print("AT+SAPBR=1,1\n");
      GSM.process++;
      break;
    case 7:
      checkSerial("OK", "ERROR");
      break;
    case 8:
      Serial1.print("AT+SAPBR=2,1\n");
      GSM.process++;
      break;
    case 9:
      checkSerial("OK", "ERROR");
      break;
    case 10:
      Serial1.print("AT+CIPGSMLOC=1,1\n");
      GSM.process++;
      break;
    case 11:
      checkSerial("OK", "ERROR");
      break;
    case 12:
      if (!GSM.processSuccess) {
        GSM.powerUp = 1;
      }
      else {
        GSM.firstPost = 1;
        GSM.post = 1;
      }
      GSM.process = 0;
      GSM.location = 0;
      GSM.readLongitud = 0;
      GSM.readLatitud = 0;
      break;
  }
}

void checkFirstPost() {
  if (!GSM.firstPost && GSM.connectionStatus) {
    GSM.location = 1;
  }
}

void checkGSMConnection() {
  switch (GSM.process) {
    case 0:
      GSM.processSuccess = 1;
      Serial1.print("AT\n");
      GSM.process++;
      break;
    case 1:
      checkSerial("OK", "ERROR");
      break;
    case 2:
      if (!GSM.processSuccess) {
        GSM.powerUp = 1;
      }
      GSM.process = 0;
      break;
  }
}

void GSMPowerUp() {
  switch (GSM.process) {
    case 0:
      GSM.processTime = millis();
      digitalWrite(GSM_PWRKEY, LOW);
      GSM.processSuccess = 1;
      GSM.process++;
      GSM.packetSentenceTime = millis();
      logln("powering up GSM");
      break;
    case 1:
      if (millis() - GSM.packetSentenceTime > 2000) {
        digitalWrite(GSM_PWRKEY, HIGH);
        GSM.packetSentenceTime = millis();
        GSM.process++;
        logln("GSM powered");
      }
      break;
    case 2:
      if (millis() - GSM.packetSentenceTime > 8000) {
        Serial1.print("AT\n");
        GSM.process++;
        GSM.packetSentenceTime = millis();
        logln("Sending AT command");
      }
      break;
    case 3:
      if (millis() - GSM.packetSentenceTime > 1000) {
        Serial1.print("AT\n");
        GSM.packetSentenceTime = millis();
      }
      checkSerial("OK", "ERROR");
      break;
    case 4:
      if (GSM.processSuccess) {
        GSM.powerUp = 0;
        GSM.connect = 1;
        logln("Power up success");
      }
      GSM.location = 0;
      GSM.process = 0;
      break;
  }
}

void GSMStablishConnection() {
  switch (GSM.process) {
    case 0:
      GSM.processTime = millis();
      GSM.processSuccess = 1;
      Serial1.write("AT+CFUN=1\n");
      GSM.process++;
      break;
    case 1:
      checkSerial("OK", "ERROR");
      break;
    case 2:
      Serial1.write("AT+CPIN?\n");
      GSM.process++;
      break;
    case 3:
      checkSerial("OK", "ERROR");
      break;
    case 4:
      Serial1.write("AT+CSTT=\"TM\",\"\",\"\"\n\n");
      GSM.process++;
      break;
    case 5:
      checkSerial("OK", "ERROR");
      break;
    case 6:
      Serial1.write("AT+CIICR\n");
      GSM.process++;
      break;
    case 7:
      checkSerial("OK", "ERROR");
      break;
    case 8:
      Serial1.write("AT+CIFSR\n");
      GSM.process++;
      break;
    case 9:
      Serial1.write("AT\n");
      GSM.process++;
      break;
    case 10:
      checkSerial("OK", "ERROR");
      break;
    case 11:
      if (GSM.processSuccess) {
        GSM.connectionStatus = 1;
      }
      else {
        GSM.powerUp = 1;
      }
      GSM.connect = 0;
      GSM.process = 0;
      break;
  }
}

void GSMPost() {
  switch (GSM.process) {
    case 0:
      GSM.processTime = millis();
      GSM.processSuccess = 1;
      GSM.lastSent = millis();
      Serial1.print("AT+CIPSTART=\"TCP\",\"" + server + "\",80\n");
      GSM.process++;
      SDlog(); //log GSM data in SD module
      break;
    case 1:
      checkSerial("CONNECT OK", "ERROR");
      break;
    case 2:
      len = 0;
      for (int i = 0; i < 7; i++) {
        len += req[i].length();
      }
      Serial1.print("AT+CIPSEND=" + String(len - 1) + "\n");
      GSM.process++;
      break;
    case 3:
      if (millis() - GSM.packetSentenceTime > 200) {
        Serial1.print(req[GSMsequence]);
        GSMsequence++;
        if (GSMsequence == 7) {
          GSM.process++;
          GSMsequence = 0;
        }
        GSM.packetSentenceTime = millis();
      }
      break;
    case 4:
      checkSerial("SEND OK", "\n\n");
      break;
    case 5:
      checkSerial("CLOSED", "\n\n", "ey");
      break;
    case 6:
      if (GSMsequence == 0) {
        GSM.token.trim();
        postGSMVariables();
        req2[3] = "Authorization: Bearer " + GSM.token + "\n";
        req2[4] = "Content-Length: " + String(req2[7].length()) + "\n";
        len = 0;
      }
      len += req2[GSMsequence].length();
      GSMsequence++;
      if (GSMsequence == 8) {
        Serial1.print("AT+CIPSTART=\"TCP\",\"" + server + "\",80\n");
        GSM.process++;
        GSMsequence = 0;
      }
      GSM.packetSentenceTime = millis();
      break;
    case 7:
      checkSerial("CONNECT OK", "ERROR");
      break;
    case 8:
      Serial1.print("AT+CIPSEND=" + String(len - 1) + "\n");
      GSM.process++;
      break;
    case 9:
      if (millis() - GSM.packetSentenceTime > 200) {
        if (GSMsequence == 3) {
          for (int i = 0; i < req2[3].length(); i++) {
            Serial1.print(req2[3][i]);
          }
        }
        else {
          Serial1.print(req2[GSMsequence]);
        }
        GSMsequence++;
        if (GSMsequence == 8) {
          GSM.process++;
          GSMsequence = 0;
        }
        GSM.packetSentenceTime = millis();
      }
      break;
    case 10:
      checkSerial("CLOSED", "\n\n");
      break;
    case 11:
      logln("GSM POST SUCCESS");
      GSM.post = 0;
      GSM.process = 0;
      break;
  }
}

void readGSMData() {
  if (GSM.enable) {
    while (Serial1.available()) {
      if (GSM.bufferWritePos > 1023) {
        GSM.bufferWritePos -= 1024;
        logln("Buffer overflow");
      }
      GSM.buffer[GSM.bufferWritePos] = Serial1.read();
      if (!GSM.post) {
        log(String(GSM.buffer[GSM.bufferWritePos]));
      }
      if (GSM.post) {
        if (!GSM.readToken) {
          if (GSM.buffer[GSM.bufferWritePos] == char('y')) {
            if (GSM.buffer[GSM.bufferWritePos - 1] == char('e')) {
              GSM.readToken = 1;
              GSM.token = "\n";
              GSM.token += "ey";
            }
          }
        }
        else {
          if (GSM.buffer[GSM.bufferWritePos] == '\n') {
            GSM.readToken = 0;
          }
          else {
            GSM.token += GSM.buffer[GSM.bufferWritePos];
          }
        }
      }
      else if (GSM.location) {
        if (GSM.process == 11) {
          if (GSM.buffer[GSM.bufferWritePos] == char(',') ) {
            if (GSM.buffer[GSM.bufferWritePos - 2] != char('=')) {
              if (GSM.longitud == "") {
                GSM.readLongitud = 1;
              }
              else if (GSM.latitud == "") {
                GSM.readLongitud = 0;
                GSM.readLatitud = 1;
              }
              else if (GSM.localDayTime == "") {
                GSM.readLocalDayTime = 1;
                GSM.readLatitud = 0;
              }
              else {
                GSM.readLocalDayTime = 0;
                GSM.readLocalHourTime = 1;
              }
            }
          }
          else {
            if (GSM.readLatitud) {
              GSM.latitud += GSM.buffer[GSM.bufferWritePos];
            }
            if (GSM.readLongitud) {
              GSM.longitud += GSM.buffer[GSM.bufferWritePos];
            }
            if (GSM.readLocalDayTime) {
              GSM.localDayTime += GSM.buffer[GSM.bufferWritePos];
            }
            if (GSM.readLocalHourTime) {
              log(GSM.buffer[GSM.bufferWritePos]);
              if (GSM.buffer[GSM.bufferWritePos] == '\n') {
                GSM.readLocalHourTime = 0;
                setTime((int(GSM.localHourTime.charAt(0))-48) * 10 + (int(GSM.localHourTime.charAt(1))-48), (int(GSM.localHourTime.charAt(3))-48) * 10 + (int(GSM.localHourTime.charAt(4))-48), (int(GSM.localHourTime.charAt(6))-48) * 10 + (int(GSM.localHourTime.charAt(7))-48), (int(GSM.localDayTime.charAt(8))-48) * 10 + (int(GSM.localDayTime.charAt(9))-48), (int(GSM.localDayTime.charAt(5))-48) * 10 + (int(GSM.localDayTime.charAt(6))-48), 2000 + (int(GSM.localDayTime.charAt(2))-48) * 10 + (int(GSM.localDayTime.charAt(3))-48)); // setTime(hr,min,sec,day,month,yr); // Another way to set
                log("Local time: " + GSM.localDayTime + "," + GSM.localHourTime);
              }
              else {
                GSM.localHourTime += GSM.buffer[GSM.bufferWritePos];
              }
            }
          }
        }
      }
      GSM.bufferWritePos++;
      GSM.charToRead++;
    }
  }
}

String checkSerial(String success, String error) {
  return (checkSerial(success, error, ""));
}

String checkSerial(String success, String error, String includeOnly) {
  if (!GSM.initVars) {
    GSM.initVars = 1;
    GSM.enable = 1;
    GSM.line = "";
    GSM.lastLine = "";
  }

  if (GSM.charToRead) {
    char c = GSM.buffer[GSM.bufferPos];
    GSM.charToRead--;
    GSM.bufferPos++;
    if (GSM.bufferPos > 1023) {
      GSM.bufferPos = 0;
    }
    if (c == '\r') {
      GSM.lastLine = GSM.line;
      GSM.line = "";
      if (GSM.lastLine.equals(success) || GSM.lastLine.equals(error)) {
        if (GSM.lastLine == error) {
          //GSM.error = 1;
        }
        GSM.initVars = 0;
        GSM.process++;
        clearGSMBuffer();
      }
    }
    else {
      if (c != '\n') {
        GSM.line = GSM.line + String(c);
      }
    }
  }
  return GSM.line;
}

void clearGSMBuffer() {
  while (Serial1.available()) {
    logln("Not reading char: " + Serial1.read());
  }
  GSM.charToRead = 0;
  GSM.bufferPos = 0;
  GSM.bufferWritePos = 0;
}

void postGSMVariables() {
  req2[7] = "{";
  if (GSM.postBabyTemp) {
    req2[7] += "\"temperature\":\"" + String(temperature[babyNTC], 2) + "\"";
  }
  if (GSM.postHumidity) {
    if (req2[7] != "{") {
      req2[7] += ",";
    }
    req2[7] += "\"humidity\":\"" + String(humidity) + "\"";
  }
  req2[7] += "}\n";
}
