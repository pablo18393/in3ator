#define TOKEN "tOUZTeBHHycW5BGntTuO"
#define THINGSBOARD_SERVER "mon.medicalopenworld.org"


// Baud rate for debug serial
#define SERIAL_DEBUG_BAUD 115200
#define MODEM_BAUD 115200
#define RX_BUFFER_LENGTH 1024
#define GPRS_TIMEOUT 60000 // in millisecs

#define GPRS_CONNECTION_TIMEOUT 2000

#define modemSerial Serial2
#define DEFAULT_POST_PERIOD 10

#define standByGPRSPostPeriod 30
#define actuatingGPRSPostPeriod 30
#define jaundiceGPRSPostPeriod 30
#define GPRS_SHUT OFF
#define NO_COMMENT false

#define APN "TM"
#define APN_TM "\"TM\"""\n"
#define APN_TRUPHONE "\"iot.truphone.com\"""\n"
#define GPRS_USER ""
#define GPRS_PASS ""

#define SIMCOM800_AT "AT\n"
#define SIMCOM800_AT_CFUN "AT+CFUN=1\n"
#define SIMCOM800_AT_CSQ "AT+CSQ\n"
#define SIMCOM800_AT_STATUS "AT+CPIN?\n"
#define SIMCOM800_AT_CCID "AT+CCID\n"
#define SIMCOM800_AT_CIICR "AT+CIICR\n"
#define SIMCOM800_AT_CIFSR_E0 "AT+CIFSR;E0\n"
#define SIMCOM800_AT_CIFSR "AT+CIFSR\n"

#define SIMCOM800_CMEE_0 "AT+CMEE=0\n"
#define SIMCOM800_CLTS_1 "AT+CLTS=1\n"
#define SIMCOM800_CBAT_1 "AT+CBATCHK=1\n"
#define SIMCOM800_AT_CREG_ASK "AT+CREG?\n"
#define SIMCOM800_AT_CGACT_1 "AT+CGACT=1,1\n"
#define SIMCOM800_AT_SAPBR_1 "AT+SAPBR=1,1\n"
#define SIMCOM800_AT_SAPBR_2 "AT+SAPBR=2,1\n"
#define SIMCOM800_AT_CIPSHUT "AT+CIPSHUT\n"
#define SIMCOM800_AT_SAPBR "AT+SAPBR=3,1,\"Contype\",\"GPRS\"\n"
#define SIMCOM800_SET_APN_TM "AT+SAPBR=3,1,\"APN\"," APN_TM 
#define SIMCOM800_SET_APN_TRUPHONE "AT+SAPBR=3,1,\"APN\"," APN_TRUPHONE
#define SIMCOM800_SET_PDP_TM "AT+CGDCONT=1,\"IP\"," APN_TM
#define SIMCOM800_SET_PDP_TRUPHONE "AT+CGDCONT=1,\"IP\"," APN_TRUPHONE
#define SIMCOM800_AT_CGATT_0 "AT+CGATT=0\n"
#define SIMCOM800_AT_CGATT_1 "AT+CGATT=1\n"
#define SIMCOM800_AT_CIPMUX_1 "AT+CIPMUX=1\n"
#define SIMCOM800_AT_CIPSEND_1 "AT+CIPQSEND=1\n"
#define SIMCOM800_AT_CIPRXGET_1 "AT+CIPRXGET=1\n"
#define SIMCOM800_AT_CSTT_TM "AT+CSTT=\"" APN_TM "\",\"\",\"\"\n"
#define SIMCOM800_AT_CSTT_TRUPHONE "AT+CSTT=\"" APN_TRUPHONE "\",\"\",\"\"\n"
#define SIMCOM800_AT_DNS "AT+CDNSCFG=\"8.8.8.8\",\"8.8.4.4\"\n"

#define AT_OK "OK"
#define AT_ERROR "ERROR"
#define AT_CREG_OK "+CREG: 0,5"