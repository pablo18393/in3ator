
String user = "admin@admin.com";
String password = "admin";
String token = "";
int temp = 10;
int hum = 12;
String server = "pub.scar.io";

int GPRSPartialMessageLength;
bool newData = 0;
int uartBufferSize = 3000;
char GSMRXBuffer[3000];
int bufferReadPos = 0;
int newBufferPos = 0;

//GPRS variables
#define environmentalWords[] = "TimeOn","Temperature","Humidity","DesiredTemp","HeaterPower","PIDOutput","maxHeaterTemp";
#define ENC_SWITCHSensorVariablesWords[] = "BPM","IBI";
#define ENC_SWITCHSensorRawWords[] = "ENC_SWITCHMeasurements";
#define aliveRefreshWords[] = "timeOn","LEDsON";

bool initGPRS() {
  Serial1.begin(115200);
  attachInterrupt(GSMRXPIN, GSMISR, FALLING);
  bool initOK = 0;
  powerGSM();
  connectToInternet();
  return (initOK);
}

void powerGSM() {
  digitalWrite(GSM_PWRKEY, HIGH);
  delay(2000);
  digitalWrite(GSM_PWRKEY, LOW);
}

void connectToInternet() {
  Serial1.write("AT\n");
  checkSerial("OK", "ERROR");
  Serial1.write("AT+CFUN=1\n");
  checkSerial("OK", "ERROR");
  Serial1.write("AT+CPIN?\n");
  checkSerial("OK", "ERROR");
  Serial1.write("AT+CSTT=\"Terminal\",\"\",\"\"\n");
  checkSerial("OK", "ERROR");
  Serial1.write("AT+CIICR\n");
  checkSerial("OK", "ERROR");
  Serial1.write("AT+CIFSR\n");
  checkSerial("\n", "");
}

bool logIn() {
  token.trim();
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
  req2[3] = "Authorization: Bearer " + token + "\n";
  req2[4] = "Content-Length: " + String(req2[6].length() - 1) + "\n";
  int len = 0;
  for (int i = 0; i < 8; i++) {
    len += req2[i].length();
    Serial.print(String(i) + String(req2[i].length()) + ":" + req2[i]);
  }
  Serial2.print("AT+CIPSTART=\"TCP\",\"" + server + "\",80\n");
  checkSerial("CONNECT OK", "ERROR");
  delay(1000);
  Serial2.print("AT+CIPSEND=" + String(len - 1) + "\n");
  checkSerial("ERROR", "");
  delay(200);
  for (int i = 0; i < 8; i++) {
    Serial2.print(req2[i]);
    if (i < 7) {
      checkSerial("", "\n");
    }
  }
  checkSerial("SEND OK", "\n\n");
  String rec = checkSerial("CLOSED", "\n\n");
}


bool postGPRS(int variable) {
  String req[] = {
    "POST /In3/public/api/v1/auth/login?c=1 HTTP/1.1\n",
    "Host: " + server + "\n",
    "Content-Type: application/x-www-form-urlencoded\n",
    "Content-Length: 46\n",
    "Connection: close\n",
    "\n",
    "{\"email\":\"" + user + "\",\"password\":\"" + password + "\"}\n"
  };
  req[3] = "Content-Length: " + String(req[6].length() - 1) + "\n";

  Serial1.print("AT+CIPSTART=\"TCP\",\"" + server + "\",80\n");
  checkSerial("CONNECT OK", "ERROR");
  delay(1000);
  int len = 0;
  for (int i = 0; i < 7; i++) {
    len += req[i].length();
  }
  Serial1.print("AT+CIPSEND=" + String(len - 1) + "\n");
  checkSerial("ERROR", "");
  delay(200);
  for (int i = 0; i < 7; i++) {
    Serial1.print(req[i]);
    if (i < 6) {
      checkSerial("", "\n");
    }
  }
  checkSerial("SEND OK", "\n\n");
  token = checkSerial("CLOSED", "\n\n", "ey");
  Serial.println("token: " + token);

  /*
    switch (variable) {
    case environmental:
      break;
    case ENC_SWITCHSensorVariables:
      break;
    case ENC_SWITCHSensorRaw:
      break;
    case turnedOn:
      break;
    }
  */
  if (variable == environmental) {

  }
}

char readGSMbuffer() {

  return(true);
}

void GSMISR() {
  while (Serial1.available()) {
    newData = true;
    GSMRXBuffer[newBufferPos] = Serial1.read();
    if (newBufferPos < uartBufferSize) {
      newBufferPos++;
    }
    else {
      newBufferPos = 0;
    }
  }
}

String checkSerial(String success, String error) {
  return checkSerial(success, error, "");
}

String checkSerial(String success, String error, String includeOnly) {
  String a = "";
  String line = "";
  String lastLine = "";
  while (!Serial1.available());
  boolean discard = false;
  while (lastLine != success && lastLine != error) {
    char c = Serial1.read();
    if (c == '\n') {
      if (line.indexOf(includeOnly) >= 0) {
        Serial.println(line);
        a = a + "\n" + line;
      }
      line = "";
    } else {
      if (line != "") {
        lastLine = line;
      }
      line = line + String(c);
    }
  }
  if (line != "") {
    if (line.indexOf(includeOnly) >= 0) {
      a = a + "\n" + line;
      Serial.println(">>" + line);
    }
  }
  return a;
}
