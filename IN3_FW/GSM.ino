// buffer de 768 para el serial

#define GSMRXPIN 25

String user = "admin@admin.com";
String password = "admin";
String token = "";
int temp = 12;
int hum = 34;
String server = "pub.scar.io";
char GSMRXBuffer[1024];
struct GSMstruct {
  char buffer[1024];
  int charToRead;
  int bufferPos;
};

struct GSMstruct GSM;

void initGPRS()
{
  // Open serial communications and wait for port to open:
  Serial.begin(115200);

  // set the data rate for the SoftwareSerial port
  Serial1.begin(115200);
  attachInterrupt(GSMRXPIN, GSMISR, CHANGE);
  GSMPowerUp();
  //connectoToInternet();
  //GSMPost();
}

void GSMPowerUp() {
  digitalWrite(GSM_PWRKEY, HIGH);
  delay(2000);
  digitalWrite(GSM_PWRKEY, LOW);
}

void connectoToInternet() {
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

void GSMPost() {
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
    delay(200);
  }
  checkSerial("SEND OK", "\n\n");
  token = checkSerial("CLOSED", "\n\n", "ey");

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
  req2[4] = "Content-Length: " + String(req2[7].length()) + "\n";
  len = 0;
  for (int i = 0; i < 8; i++) {
    len += req2[i].length();
    Serial.print(String(i) + String(req2[i].length()) + ":" + req2[i]);
    delay(100);
  }
  Serial1.print("AT+CIPSTART=\"TCP\",\"" + server + "\",80\n");
  checkSerial("CONNECT OK", "ERROR");
  delay(1000);
  Serial1.print("AT+CIPSEND=" + String(len - 1) + "\n");
  checkSerial("ERROR", "");
  delay(200);
  for (int i = 0; i < 8; i++) {
    Serial1.print(req2[i]);
    if (i < 7) {
      checkSerial("", "\n");
    }
    delay(100);
  }
  checkSerial("SEND OK", "\n\n");
  if (checkSerial("HTTP/1.1 200 OK", "\n\n") == "HTTP/1.1 200 OK\n\n") {
    Serial.println("COMMUNICATION SUCCESSFULL");
  }
  else {
    Serial.println("FAIL");
  }
}

String checkSerial(String success, String error) {
  return checkSerial(success, error, "");
}

String checkSerial(String success, String error, String includeOnly) {
  String a = "";
  String line = "";
  String lastLine = "";
  clearGSMBuffer();
  while (!Serial1.available());
  boolean discard = false;
  char c;
  while (lastLine != success && lastLine != error) {
    GSMISR();
    if (GSM.charToRead) {
      c = GSM.buffer[GSM.bufferPos];
      GSM.charToRead--;
      GSM.bufferPos++;
      if (GSM.bufferPos > 1023) {
        GSM.bufferPos = 0;
      }
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
    delay(10);
  }
  if (lastLine == success) {
    Serial.println("success");
  }
  if (line != "") {
    if (line.indexOf(includeOnly) >= 0) {
      a = a + "\n" + line;
      Serial.println(">>" + line);
    }
  }
  return a;
}

void GSMISR() {
  if (Serial1.available()) {
    int bufferWritePos = GSM.bufferPos + GSM.charToRead;
    if (bufferWritePos > 1023) {
      bufferWritePos -= 1024;
    }
    GSM.buffer[bufferWritePos] = Serial1.read();
    GSM.charToRead++;
  }
}

void clearGSMBuffer() {
  while (Serial1.available()) {
    Serial1.read();
  }
  GSM.charToRead = 0;
  GSM.bufferPos = 0;
}
