
String user = "admin@admin.com";
String password = "admin";
String token = "";
int temp = 10;
int hum = 12;
String server = "pub.scar.io";

int GPRSPartialMessageLength;
bool newData = 0;
char uartBuffer[3000];
int uartCounter = 0;

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
  Serial2.write("AT\n");
  checkSerial("OK", "ERROR");
  Serial2.write("AT+CFUN=1\n");
  checkSerial("OK", "ERROR");
  Serial2.write("AT+CPIN?\n");
  checkSerial("OK", "ERROR");
  Serial2.write("AT+CSTT=\"Terminal\",\"\",\"\"\n");
  checkSerial("OK", "ERROR");
  Serial2.write("AT+CIICR\n");
  checkSerial("OK", "ERROR");
  Serial2.write("AT+CIFSR\n");
  checkSerial("\n", "");
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

  Serial2.print("AT+CIPSTART=\"TCP\",\"" + server + "\",80\n");
  checkSerial("CONNECT OK", "ERROR");
  delay(1000);
  int len = 0;
  for (int i = 0; i < 7; i++) {
    len += req[i].length();
  }
  Serial2.print("AT+CIPSEND=" + String(len - 1) + "\n");
  checkSerial("ERROR", "");
  delay(200);
  for (int i = 0; i < 7; i++) {
    Serial2.print(req[i]);
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

void GSMISR() {
  while (Serial1.available()) {
    newData = 1;
    uartBuffer[uartCounter] = Serial1.read();
    if (uartCounter < 999) {
      uartCounter++;
    }
    else {
      uartCounter = 0;
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
  while (!Serial2.available());
  //while (Serial2.available()) {
  boolean discard = false;
  while (lastLine != success && lastLine != error) {
    char c = Serial2.read();
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
