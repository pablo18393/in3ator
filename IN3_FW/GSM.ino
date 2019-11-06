// buffer de 768 para el serial

String user = "admin@admin.com";
String password = "admin";
long temp = 12;
long hum = 34;
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
  char buffer[1024];
  int charToRead;
  int bufferPos;
  int bufferWritePos;
  String token;
  String line;
  String lastLine;
  bool readToken;
  bool enable;
  bool initVars;
  bool connect;
  byte GSMProcess;
  bool timeOut;
  long processTime;
  long packetSentenceTime;
  bool post;
  byte postProcess;
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
};

int GSMsequence = 0;

struct GSMstruct GSM;

void initGPRS()
{
  Serial.begin(115200);
  Serial1.begin(115200);
  GSMTimer.pause();
  GSMTimer.setPeriod(GSMISRRate); // in microseconds
  GSMTimer.setChannel1Mode(TIMER_OUTPUT_COMPARE);
  GSMTimer.setCompare(TIMER_CH2, 1);  // Interrupt 1 count after each update
  GSMTimer.attachCompare1Interrupt(GSMHandler);
  GSMTimer.refresh();
  GSMTimer.resume();
  //attachInterrupt(GSMRXPIN, GSMreadSerial, CHANGE);
  while (!Serial.available());
  GSM.connect = 1;
  while (1) {
    GSMreadSerial();
    if (Serial.available()) {
      GSM.GSMProcess = 0;
      while (Serial.available()) {
        Serial.read();
      }
    }
    if (!GSM.connect) {
      GSM.post = 1;
    }
  }
}

void GSMHandler() {
  if (GSM.connect) {
    switch (GSM.GSMProcess) {
      case 0:
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
        Serial1.write("AT\n");
        GSM.GSMProcess++;
        break;
      case 9:
        checkSerial("OK", "ERROR");
        break;
      case 10:
        GSM.connect = 0;
        GSM.GSMProcess = 0;
        Serial.println("GSM connection successful");
        break;
    }
  }
  if (GSM.post) {
    switch (GSM.GSMProcess) {
      case 0:
        Serial1.print("AT+CIPSTART=\"TCP\",\"" + server + "\",80\n");
        GSM.GSMProcess++;
        break;
      case 1:
        checkSerial("CONNECT OK", "ERROR");
        break;
      case 2:
        for (int i = 0; i < 7; i++) {
          len += req[i].length();
        }
        Serial1.print("AT+CIPSEND=" + String(len - 1) + "\n");
        GSM.GSMProcess++;
        break;
      case 3:
        checkSerial("ERROR", "");
        GSMsequence = 0;
        break;
      case 4:
        if (millis() - GSM.packetSentenceTime > 100) {
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
        if (millis() - GSM.packetSentenceTime > 100) {
          if (GSMsequence == 0) {
            GSM.token.trim();
            req2[3] = "Authorization: Bearer " + GSM.token + "\n";
            req2[4] = "Content-Length: " + String(req2[7].length()) + "\n";
            len = 0;
          }
          len += req2[GSMsequence].length();
          Serial.print(String(GSMsequence) + String(req2[GSMsequence].length()) + ":" + req2[GSMsequence]);
          GSMsequence++;
          if (GSMsequence == 8) {
            Serial1.print("AT+CIPSTART=\"TCP\",\"" + server + "\",80\n");
            GSM.GSMProcess++;
            GSMsequence = 0;
          }
          GSM.packetSentenceTime = millis();
        }
        break;
      case 8:
        checkSerial("CONNECT OK", "ERROR");
        break;
      case 9:
        Serial1.print("AT+CIPSEND=" + String(len - 1) + "\n");
        GSM.GSMProcess++;
        break;
      case 10:
        checkSerial("ERROR", "");
        break;
      case 11:
        if (millis() - GSM.packetSentenceTime > 100) {
          GSMTimer.pause();
          GSMTimer.setPeriod(300000); // in microseconds
          GSMTimer.resume();
          Serial1.print(req2[GSMsequence]);
          GSMsequence++;
          if (GSMsequence == 8) {
            GSMTimer.pause();
            GSMTimer.setPeriod(GSMISRRate); // in microseconds
            GSMTimer.resume();
            GSM.GSMProcess++;
            GSMsequence = 0;
            Serial.println("transmitted");
          }
          GSM.packetSentenceTime = millis();
        }
        break;
      case 12:
        checkSerial("SEND OK", "\n\n");
        break;
      case 13:
        checkSerial("HTTP/1.1 200 OK", "\n\n");
        break;
      case 14:
        GSM.post = 0;
        break;
    }
  }
  if (GSM.enable) {
    //GSMreadSerial();
  }
}

void GSMreadSerial() {
  if (Serial1.available()) {
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
        Serial.println("Token is: " + GSM.token);
      }
      else {
        GSM.token += GSM.buffer[GSM.bufferWritePos];
      }
    }
    GSM.bufferWritePos++;
    GSM.charToRead++;
  }
}

void GSMPowerUp() {
  digitalWrite(GSM_PWRKEY, HIGH);
  delay(2000);
  digitalWrite(GSM_PWRKEY, LOW);
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
    Serial.println("GSM variables reset");
  }

  if (millis() - GSM.processTime > GSMTimeOut * 1000) {
    GSM.timeOut = 1;
    Serial.println("timeOut");
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
          Serial.println("<<" + GSM.line);
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
    if (GSM.line != "") {
      if (GSM.line.indexOf(includeOnly) >= 0) {
        Serial.println(">>" + GSM.line);
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
    Serial.println("Not reading" + Serial1.read());
  }
  GSM.charToRead = 0;
  GSM.bufferPos = 0;
  GSM.bufferWritePos = 0;
}
