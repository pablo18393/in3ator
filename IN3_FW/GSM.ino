// buffer de 768 para el serial

String user = "admin@admin.com";
String password = "admin";
float temp = 25.56;
float hum = 29.87;
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
  "{\"temperature\":\"" + String(temp) + "\",\"humidity\":\"" + String(hum) + "\"}\n"
};

int len = 0;
long GSMTimeOut = 30; //in secs
struct GSMstruct {
  bool firstPost;
  bool error;
  bool SN;
  bool babyTemp;
  bool HeaterTemp;
  bool BoardTemp1;
  bool BoardTemp2;
  bool BoardTemp3;
  bool Humidity;
  bool Longitud;
  bool Latitud;
  bool JaundicePower;
  bool BPM;
  bool IBI;
  bool RPS;
  bool HeaterPower;
  int sendPeriod;
  long lastSent;
  char buffer[1024];
  int charToRead;
  int bufferPos;
  int bufferWritePos;
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
  byte GSMProcess;
  long processTime;
  long packetSentenceTime;
  bool post;
  byte postProcess;
};

int GSMsequence = 0;

struct GSMstruct GSM;

void initGPRS()
{
  //Serial.end();
  Serial1.begin(115200);
  GSM.sendPeriod = 600; //in secs
  GSM.powerUp = 1;
}

void GSMHandler() {
  if (GSM.powerUp) {
    GSMPowerUp();
  }
  if (GSM.connect) {
    GSMStablishConnection();
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
    GSM.GSMProcess = 0;
    GSM.post = 0;
    GSM.connect = 0;
    GSM.powerUp = 1;
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
  switch (GSM.GSMProcess) {
    case 0:
      GSM.processSuccess = 1;
      Serial1.print("AT+CGATT=1\n");
      GSM.GSMProcess++;
      break;
    case 1:
      checkSerial("OK", "ERROR");
      break;
    case 2:
      Serial1.print("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\n");
      GSM.GSMProcess++;
      break;
    case 3:
      checkSerial("OK", "ERROR");
      break;
    case 4:
      Serial1.print("AT+SAPBR=3,1,\"APN\",\"TM\"\n");
      GSM.GSMProcess++;
      break;
    case 5:
      checkSerial("OK", "ERROR");
      break;
    case 6:
      Serial1.print("AT+SAPBR=1,1\n");
      GSM.GSMProcess++;
      break;
    case 7:
      checkSerial("OK", "ERROR");
      break;
    case 8:
      Serial1.print("AT+SAPBR=2,1\n");
      GSM.GSMProcess++;
      break;
    case 9:
      checkSerial("OK", "ERROR");
      break;
    case 10:
      Serial1.print("AT+CIPGSMLOC=1,1\n");
      GSM.GSMProcess++;
      break;
    case 11:
      if (!GSM.processSuccess) {
        GSM.powerUp = 1;
      }
      GSM.GSMProcess = 0;
      break;
  }
}

void checkFirstPost() {
  if (!GSM.firstPost && GSM.connectionStatus) {
      //GSMLocation();
      GSM.firstPost = 1;
      GSM.post = 1;
  }
}

void checkGSMConnection() {
  switch (GSM.GSMProcess) {
    case 0:
      GSM.processSuccess = 1;
      Serial1.print("AT\n");
      GSM.GSMProcess++;
      break;
    case 1:
      checkSerial("OK", "ERROR");
      break;
    case 2:
      if (!GSM.processSuccess) {
        GSM.powerUp = 1;
      }
      GSM.GSMProcess = 0;
      break;
  }
}

void GSMPowerUp() {
  switch (GSM.GSMProcess) {
    case 0:
      digitalWrite(GSM_PWRKEY, HIGH);
      GSM.processSuccess = 1;
      GSM.GSMProcess++;
      GSM.packetSentenceTime = millis();
      break;
    case 1:
      if (millis() - GSM.packetSentenceTime > 2000) {
        digitalWrite(GSM_PWRKEY, LOW);
        GSM.packetSentenceTime = millis();
        GSM.GSMProcess++;
        GSM.powerUp = 0;
        GSM.connect = 1;
        GSM.GSMProcess = 0;
      }
      break;
    case 2:
      if (millis() - GSM.packetSentenceTime > 2000) {
        Serial1.print("AT\n");
        GSM.GSMProcess++;
      }
      break;
    case 3:
      checkSerial("OK", "ERROR");
      break;
    case 4:
      if (GSM.processSuccess) {
        GSM.powerUp = 0;
        GSM.connect = 1;
      }
      GSM.GSMProcess = 0;
      break;
  }
}

void GSMStablishConnection() {
  switch (GSM.GSMProcess) {
    case 0:
      GSM.processSuccess = 1;
      Serial1.write("AT+CFUN=1\n");
      GSM.GSMProcess++;
      break;
    case 1:
      checkSerial("OK", "ERROR");
      break;
    case 2:
      Serial1.write("AT+CPIN?\n");
      GSM.GSMProcess++;
      break;
    case 3:
      checkSerial("OK", "ERROR");
      break;
    case 4:
      Serial1.write("AT+CSTT=\"TM\",\"\",\"\"\n\n");
      GSM.GSMProcess++;
      break;
    case 5:
      checkSerial("OK", "ERROR");
      break;
    case 6:
      Serial1.write("AT+CIICR\n");
      GSM.GSMProcess++;
      break;
    case 7:
      checkSerial("OK", "ERROR");
      break;
    case 8:
      Serial1.write("AT+CIFSR\n");
      GSM.GSMProcess++;
      break;
    case 9:
      Serial1.write("AT\n");
      GSM.GSMProcess++;
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
      GSM.GSMProcess = 0;
      break;
  }
}

void GSMPost() {
  switch (GSM.GSMProcess) {
    case 0:
      GSM.processSuccess = 1;
      GSM.lastSent = millis();
      Serial1.print("AT+CIPSTART=\"TCP\",\"" + server + "\",80\n");
      GSM.GSMProcess++;
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
      GSM.GSMProcess++;
      break;
    case 3:
      checkSerial(">", "ERROR");
      GSMsequence = 0;
      break;
    case 4:
      if (millis() - GSM.packetSentenceTime > 200) {
        Serial1.print(req[GSMsequence]);
        GSMsequence++;
        if (GSMsequence == 7) {
          GSM.GSMProcess++;
          GSMsequence = 0;
        }
        GSM.packetSentenceTime = millis();
      }
      break;
    case 5:
      checkSerial("SEND OK", "\n\n");
      break;
    case 6:
      checkSerial("CLOSED", "\n\n", "ey");
      break;
    case 7:
      if (GSMsequence == 0) {
        GSM.token.trim();
        req2[3] = "Authorization: Bearer " + GSM.token + "\n";
        req2[4] = "Content-Length: " + String(req2[7].length()) + "\n";
        len = 0;
      }
      len += req2[GSMsequence].length();
      GSMsequence++;
      if (GSMsequence == 8) {
        Serial1.print("AT+CIPSTART=\"TCP\",\"" + server + "\",80\n");
        GSM.GSMProcess++;
        GSMsequence = 0;
      }
      GSM.packetSentenceTime = millis();
      break;
    case 8:
      checkSerial("CONNECT OK", "ERROR");
      break;
    case 9:
      Serial1.print("AT+CIPSEND=" + String(len - 1) + "\n");
      GSM.GSMProcess++;
      break;
    case 10:
      checkSerial(">", "ERROR");
      break;
    case 11:
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
          GSM.GSMProcess++;
          GSMsequence = 0;
        }
        GSM.packetSentenceTime = millis();
      }
      break;
    case 12:
      checkSerial("SEND OK", "\n\n");
      break;
    case 13:
      checkSerial("CLOSED", "\n\n");
      break;
    case 14:
      Serial.println("GSM POST SUCCESS");
      GSM.post = 0;
      GSM.GSMProcess = 0;
      break;
  }
}

void readGSMData() {
  if (GSM.enable) {
    while (Serial1.available()) {
      if (GSM.bufferWritePos > 1023) {
        GSM.bufferWritePos -= 1024;
        Serial.println("Buffer overflow");
      }
      GSM.buffer[GSM.bufferWritePos] = Serial1.read();
      Serial.print(GSM.buffer[GSM.bufferWritePos]);
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
    GSM.processTime = millis();
  }

  if (millis() - GSM.processTime > GSMTimeOut * 1000) {
    GSM.timeOut = 1;
    Serial.println("timeOut");
    Serial.println(String(GSM.connect) + String(GSM.powerUp) + String(GSM.post) + String(GSM.GSMProcess));
    GSM.processTime = millis();
  }

  if (GSM.lastLine != success && GSM.lastLine != error) {
    if (GSM.charToRead) {
      char c = GSM.buffer[GSM.bufferPos];
      GSM.charToRead--;
      GSM.bufferPos++;
      if (GSM.bufferPos > 1023) {
        GSM.bufferPos = 0;
      }
      if (c == '\n') {
        if (GSM.line.indexOf(includeOnly) >= 0) {
        }
        GSM.line = "";
      } else {
        if (GSM.line != "") {
          GSM.lastLine = GSM.line;
        }
        GSM.line = GSM.line + String(c);
      }
    }
  }
  else {
    if (GSM.lastLine == error) {
      //GSM.error = 1;
    }
    if (GSM.line != "") {
      if (GSM.line.indexOf(includeOnly) >= 0) {
      }
    }
    GSM.initVars = 0;
    GSM.GSMProcess++;
    clearGSMBuffer();
  }
  return GSM.line;
}

void clearGSMBuffer() {
  while (Serial1.available()) {
    Serial.println("Not reading char: " + Serial1.read());
  }
  GSM.charToRead = 0;
  GSM.bufferPos = 0;
  GSM.bufferWritePos = 0;
}
