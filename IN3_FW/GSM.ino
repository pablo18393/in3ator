// buffer de 768 para el serial

String user = "admin@admin.com";
String password = "admin";
String token = "";
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
  String token;
  String line;
  String lastLine;
  volatile bool enable;
  volatile bool initVars;
  volatile bool connect;
  volatile byte GSMProcess;
  volatile bool timeOut;
  volatile long processTime;
  volatile bool post;
  volatile byte postProcess;
  volatile bool SN;
  volatile bool babyTemp;
  volatile bool HeaterTemp;
  volatile bool BoardTemp1;
  volatile bool BoardTemp2;
  volatile bool BoardTemp3;
  volatile bool Humidity;
  volatile bool Longitud;
  volatile bool Latitud;
  volatile bool JaundicePower;
  volatile bool BPM;
  volatile bool IBI;
  volatile bool RPS;
  volatile bool HeaterPower;
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
  attachInterrupt(GSMRXPIN, GSMreadSerial, CHANGE);
  while (!Serial.available());
  GSM.connect = 1;
  while (1) {
    if (Serial.available()) {
      GSM.GSMProcess = 0;
      Serial.println("ping");
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
        break;
      case 4:
        Serial1.print(req[GSMsequence]);
        GSMsequence++;
        if (GSMsequence == 7) {
          GSM.GSMProcess++;
          Serial.println("sent");
          GSMsequence = 0;
        }
        break;
      case 5:
        checkSerial("SEND OK", "\n\n");
        break;
      case 6:
        token = checkSerial("CLOSED", "\n\n", "ey");
        break;
      case 7:
        token.trim();
        req2[3] = "Authorization: Bearer " + token + "\n";
        req2[4] = "Content-Length: " + String(req2[7].length()) + "\n";
        len = 0;
        for (int i = 0; i < 8; i++) {
          len += req2[i].length();
        }
        Serial1.print("AT+CIPSTART=\"TCP\",\"" + server + "\",80\n");
        GSM.GSMProcess++;
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
        Serial1.print(req2[GSMsequence]);
        GSMsequence++;
        if (GSMsequence == 8) {
          GSM.GSMProcess++;
          GSMsequence = 0;
          Serial.println("transmitted");
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
    int bufferWritePos = GSM.bufferPos + GSM.charToRead;
    if (bufferWritePos > 1023) {
      bufferWritePos -= 1024;
      //Serial.println("Buffer overflow");
    }
    GSM.buffer[bufferWritePos] = Serial1.read();
    Serial.print(GSM.buffer[bufferWritePos]);
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
    GSM.token = "";
    GSM.line = "";
    GSM.lastLine = "";
    GSM.processTime = millis();
    //clearGSMBuffer();
  }

  if (millis() - GSM.processTime > GSMTimeOut * 1000) {
    GSM.timeOut = 1;
    Serial.println("timeOut");
  }

  if (GSM.lastLine != success && GSM.lastLine != error) {
    if (GSM.charToRead) {
      detachInterrupt(GSMRXPIN);
      char c = GSM.buffer[GSM.bufferPos];
      GSM.charToRead--;
      GSM.bufferPos++;
      if (GSM.bufferPos > 1023) {
        GSM.bufferPos = 0;
      }
      attachInterrupt(GSMRXPIN, GSMreadSerial, CHANGE);
      if (c == '\n') {
        if (GSM.line.indexOf(includeOnly) >= 0) {
          GSM.token = GSM.token + "\n" + GSM.line;
        }
        GSM.line = "";
      } else {
        GSM.line = GSM.line + String(c);
        if (GSM.line != "") {
          GSM.lastLine = GSM.line;
        }
      }
    }
  }
  else {
    if (GSM.line != "") {
      if (GSM.line.indexOf(includeOnly) >= 0) {
        GSM.token = GSM.token + "\n" + GSM.line;
      }
    }
    GSM.initVars = 0;
    GSM.GSMProcess++;
    Serial.println("found" + String(GSM.GSMProcess));
  }
  return GSM.token;
}

void clearGSMBuffer() {
  while (Serial1.available()) {
    Serial.write(Serial1.read());
  }
  GSM.charToRead = 0;
  GSM.bufferPos = 0;
}
