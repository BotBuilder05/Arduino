//#include <lmic.h>
#include <hal.h>
#include <SPI.h>
#include "Device_Keys.h"
#include <SoftwareSerial.h>
SoftwareSerial* LoKyTIC;

// Schedule the transmission every 
unsigned int TX_INTERVAL = 90; //seconds

// Set duration to reset LoKy  
#define T_LoKy_reset 6 //in hour(s)

static osjob_t sendjob;
static float VccTIC;
char HHPHC;
int ISOUSC;               // intensité souscrite  
int IINST;                // intensité instantanée    en A
int PAPP;                 // puissance apparente      en VA
unsigned long HCHC;       // compteur Heures Creuses  en W
unsigned long HCHP;       // compteur Heures Pleines  en W
unsigned long BASE;       // index BASE               en W
String PTEC;              // période tarif en cours
String ADCO;              // adresse du compteur
String OPTARIF;           // option tarifaire
int IMAX;                 // intensité maxi = 90A
String MOTDETAT;          // status word
boolean teleInfoReceived;

char chksum(char *buff, uint8_t len);
boolean handleBuffer(char *bufferTeleinfo, int sequenceNumnber);

void(* resetFunc) (void) = 0; //declare reset function at address 0 - MUST BE ABOVE LOOP

// ---------------------------------------------- //
//      Read the Voltage regulated from TIC       //
long readVcc() {
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA, ADSC));
  result = ADCL;
  result |= ADCH << 8;
  result = 1126400L / result;
  return result; // Return Vcc in mV
}
// ---------------------------------------------- //

// ---------------------------------------------- //
//   Update new values from TIC for the next TX   //
void updateParameters() {
  VccTIC = (int)(readVcc()); // returns VccTIC in mVolt for LoKy decoder
//  Serial.print("VccTIC = "); Serial.print(VccTIC/1000); Serial.println(" V");
  LoKyTIC->begin(1200);  // Important!!! -> RESTART LoKyTIC 
  teleInfoReceived = readTeleInfo();
  LoKyTIC->end(); // Important!!! -> STOP LoKyTIC to send packet.
}
// ---------------------------------------------- //

// ---------------------------------------------- //
//              LoRaWAN events (LMiC)
void onEvent (ev_t ev) {
  Serial.print(os_getTime()); Serial.print(": ");
  switch (ev) {
    case EV_SCAN_TIMEOUT:
      Serial.println(F("EV_SCAN_TIMEOUT"));
      break;
    case EV_BEACON_FOUND:
      Serial.println(F("EV_BEACON_FOUND"));
      break;
    case EV_BEACON_MISSED:
      Serial.println(F("EV_BEACON_MISSED"));
      break;
    case EV_BEACON_TRACKED:
      Serial.println(F("EV_BEACON_TRACKED"));
      break;
    case EV_JOINING:
      Serial.println(F("EV_JOINING"));
      break;      
    case EV_JOINED:
      Serial.println(F("EV_JOINED"));
      LMIC_setLinkCheckMode(0);
      break;
    case EV_JOIN_FAILED:
      Serial.println(F("EV_JOIN_FAILED"));
      LMiC_Startup(); //Reset LMIC and retry
      break;
    case EV_REJOIN_FAILED:
      Serial.println(F("EV_REJOIN_FAILED"));
      LMiC_Startup(); //Reset LMIC and retry
      break;
      
    case EV_TXCOMPLETE:
      Serial.println(F("EV_TXCOMPLETE"));
      if (LMIC.txrxFlags & TXRX_ACK) Serial.println(F("Received ack"));
      if (LMIC.dataLen) { Serial.print(F("Received ")); Serial.print(LMIC.dataLen); Serial.println(F(" bytes.")); }
      delay(50);
//      setDataRate(); // Use for ADR

      // Schedule next transmission
      //Reset every T_LoKy_reset hours
      if (millis() >= (T_LoKy_reset*3600000)) resetFunc();
      do_sleep(TX_INTERVAL);
      os_setTimedCallback(&sendjob, os_getTime() + ms2osticks(TX_INTERVAL*1000), do_send);
      break;
       
    case EV_LOST_TSYNC:
      Serial.println(F("EV_LOST_TSYNC"));
      break;
    case EV_RESET:
      Serial.println(F("EV_RESET"));
      break;
    case EV_RXCOMPLETE:
      Serial.println(F("EV_RXCOMPLETE"));
      break;
    case EV_LINK_DEAD:
      Serial.println(F("EV_LINK_DEAD"));
      break;
    case EV_LINK_ALIVE:
      Serial.println(F("EV_LINK_ALIVE"));
      break;
      
    default:
      Serial.print(F("Unknown event: "));
      Serial.println((unsigned) ev);
      break;
  }
}
// ---------------------------------------------- //

// ---------------------------------------------- //
//              Verify LoKy Payload               //
void check_newValues() {
  Serial.println("");
  Serial.println("-----");
  Serial.println(" * Verify new payload ");
  unsigned long HP_pre = HCHP;
  unsigned long HC_pre = HCHC;
  unsigned long BA_pre = BASE;
  
  updateParameters();
  
  while (((HCHP <= HP_pre) && (HCHC<=HC_pre) && (BASE <= BA_pre)) || ADCO == 000000000000){
    LoKyTIC->end();
    Serial.println(" * LoKyTIC frame updating...");
    updateParameters();  continue;
  }
/*
  while (BASE < BA_pre || ADCO == 000000000000){
    LoKyTIC->end();
    Serial.println(" * LoKyTIC BASE frame updating...");
    updateParameters();  continue;
  }
*/  
  Serial.println("==> Finished checking, display info:");

  if (teleInfoReceived) { displayTeleInfo();}
}
// ---------------------------------------------- //

// ---------------------------------------------- //
//      Send the encrypted LoKy packet to TTN     //
void do_send(osjob_t* j) {
  // Check if there is not a current TX/RX job running
  if (LMIC.opmode & OP_TXRXPEND) {Serial.println(F("OP_TXRXPEND, not sending"));}
  else {
    check_newValues();
    
    int vc = VccTIC;    
    uint8_t is = IINST;
    uint16_t pa = PAPP;
    
    unsigned char loky_data[25];
    uint32_t hc = HCHC;
    uint32_t hp = HCHP;
    uint32_t be = BASE;

    // LoKy payload arrangement for sending to TTN
//    loky_data[0] = 0x0;         //data_type = 0 --> VccTIC   (2 bytes)
//    loky_data[1] = vc >> 8;
//    loky_data[2] = vc & 0xFF;
    loky_data[0] = 0x7;         //data_type = 7 --> IINST    (2 bytes)
    loky_data[1] = is >> 8;
    loky_data[2] = is & 0xFF;
    loky_data[3] = 0x9;         //data_type = 9 --> PAPP     (3 bytes)
    loky_data[4] = pa >> 16;
    loky_data[5] = pa >> 8;
    loky_data[6] = pa & 0xFF; 
    loky_data[7] = 0x04;       //data_type = 4 --> HCHP     (5 bytes)
    loky_data[8] = hc >> 32;
    loky_data[9] = hc >> 24;
    loky_data[10] = hc >> 16;
    loky_data[11] = hc >> 8;
    loky_data[12] = hc & 0xFF;
    loky_data[13]  = 0x5;       //data_type = 5 --> HCHC     (5 bytes)
    loky_data[14] = hp >> 32;
    loky_data[15] = hp >> 24;
    loky_data[16] = hp >> 16;
    loky_data[17] = hp >> 8;
    loky_data[18] = hp & 0xFF;  
    loky_data[19] = 0x08;       //data_type = 8 --> BASE     (5 bytes)
    loky_data[20] = be >> 32;
    loky_data[21] = be >> 24;
    loky_data[22] = be >> 16;
    loky_data[23] = be >> 8;
    loky_data[24] = be & 0xFF;
    LMIC_setTxData2(1, loky_data, sizeof(loky_data), 0);
    Serial.println(F("LoKy packet queued!!!"));
    
    LoKyTIC->end(); // Important!!! -> STOP LoKyTIC to send packet.
  }
  // Next TX is scheduled after TX_COMPLETE event.
}
// ---------------------------------------------- //

// ---------------------------------------------- //
//        Basic constructor for LoKyTIC           //
void TeleInfo() {const uint8_t LoKy_Rx = 6;
const uint8_t LoKy_Tx = 3; 
// LoKy_Tx is NOT used but reserved !!! (could be connected to GND)  
  LoKyTIC = new SoftwareSerial(LoKy_Rx, LoKy_Tx);
  // variables initializations
  ADCO = "000000000000";
  OPTARIF = "----";
  ISOUSC = 0;
  HCHC = 0L;
  HCHP = 0L;
  BASE = 0L;
  PTEC = "----";
  HHPHC = '-';
  IINST = 0;
  IMAX = 0;
  PAPP = 0;
  MOTDETAT = "------";
}
// ---------------------------------------------- //

// ---------------------------------------------- //
//           TIC frame capture from Linky         //
boolean readTeleInfo()  {
//boolean readTeleInfo(bool TIC_state)  {
#define startFrame  0x02
#define endFrame    0x03
#define startLine   0x0A
#define endLine     0x0D
#define maxFrameLen 280

  int comptChar=0;  // variable de comptage des caractères reçus 
  char charIn=0;    // variable de mémorisation du caractère courant en réception
  char bufferTeleinfo[21] = "";
  int bufferLen = 0;
  int checkSum;
  int sequenceNumnber = 0;    // number of information group

  //--- wait for starting frame character 
  while (charIn != startFrame)
  { // "Start Text" STX (002 h) is the beginning of the frame
    if (LoKyTIC->available())
     charIn = LoKyTIC->read()& 0x7F; // Serial.read() vide buffer au fur et à mesure
  } // fin while (tant que) pas caractère 0x02
  //--- wait for the ending frame character 
  while (charIn != endFrame)
  { // tant que des octets sont disponibles en lecture : on lit les caractères
    if (LoKyTIC->available()) {
      charIn = LoKyTIC->read()& 0x7F;
      // incrémente le compteur de caractère reçus
      comptChar++;
      if (charIn == startLine)  bufferLen = 0;
      bufferTeleinfo[bufferLen] = charIn;
      // on utilise une limite max pour éviter String trop long en cas erreur réception
      // ajoute le caractère reçu au String pour les N premiers caractères
      if (charIn == endLine)  {
        checkSum = bufferTeleinfo[bufferLen -1];
        if (chksum(bufferTeleinfo, bufferLen) == checkSum)  {// we clear the 1st character
          strncpy(&bufferTeleinfo[0], &bufferTeleinfo[1], bufferLen -3);
          bufferTeleinfo[bufferLen -3] = 0x00;
          sequenceNumnber++;
          if (! handleBuffer(bufferTeleinfo, sequenceNumnber))  {
            Serial.println(F("Sequence error ..."));
            return false;
          }
        }
        else  {
          Serial.println(F("Checksum error!"));
          return false;
        }
      }
      else 
        bufferLen++;
    }
    if (comptChar > maxFrameLen)  {
      Serial.println(F("Overflow error ..."));
      return false;
    }
  }
  return true;
}
// ---------------------------------------------- //

// ---------------------------------------------- //
//               TIC frame parsing                //
boolean handleBuffer(char *bufferTeleinfo, int sequenceNumnber) {
  // create a pointer to the first char after the space
  char* resultString = strchr(bufferTeleinfo,' ') + 1;
  boolean sequenceIsOK;

  switch(sequenceNumnber) {
    
  case 1:
    if (sequenceIsOK = bufferTeleinfo[0]=='A')  ADCO = String(resultString);
    break;
  case 2:
    if (sequenceIsOK = bufferTeleinfo[0]=='O')  OPTARIF = String(resultString);
    break;
  case 3:
    if (sequenceIsOK = bufferTeleinfo[1]=='S')  ISOUSC = atol(resultString);
    break;

  case 4:
    if (sequenceIsOK = bufferTeleinfo[3]=='C'){ 
      #define Linky_HCHP true
        #ifdef Linky_HCHP
        HCHC = atol(resultString);
        #endif
    }
    else if (sequenceIsOK = bufferTeleinfo[0]=='B') { 
      #define Linky_BASE true
        #ifdef Linky_BASE
        BASE = atol(resultString);
        #endif

    }
    break;

  case 5:
    if (sequenceIsOK = bufferTeleinfo[3]=='P'){ 
      #define Linky_HCHP true
        #ifdef Linky_HCHP
        HCHP = atol(resultString);
        #endif
    }
    else if (sequenceIsOK = bufferTeleinfo[1]=='T') { 
      #define Linky_BASE true
        #ifdef Linky_BASE
        PTEC = String(resultString);
        #endif
    }
    break;

  case 6:
    if (sequenceIsOK = bufferTeleinfo[1]=='T'){ 
      #define Linky_HCHP true
        #ifdef Linky_HCHP
        PTEC = String(resultString);
        #endif
        }
    else if (sequenceIsOK = bufferTeleinfo[1]=='I') { 
      #define Linky_BASE true
        #ifdef Linky_BASE
        IINST = atol(resultString);
        #endif
        }
    break;
        
  case 7:
    if (sequenceIsOK = bufferTeleinfo[1]=='I'){ 
      #define Linky_HCHP true
        #ifdef Linky_HCHP
        IINST =atol(resultString);
        #endif
        }
    else if (sequenceIsOK = bufferTeleinfo[1]=='M') { 
      #define Linky_BASE true
        #ifdef Linky_BASE
        IMAX =atol(resultString);
        #endif
        }
    break;

  case 8:
    if (sequenceIsOK = bufferTeleinfo[1]=='M') {
      #define Linky_HCHP true
        #ifdef Linky_HCHP
        IMAX =atol(resultString);
        #endif
        }
    else if (sequenceIsOK = bufferTeleinfo[1]=='A') { 
      #define Linky_BASE true
        #ifdef Linky_BASE
        PAPP =atol(resultString);
        #endif
        }
    break;
        
  case 9:
    if (sequenceIsOK = bufferTeleinfo[1]=='A') {
      #define Linky_HCHP true
        #ifdef Linky_HCHP
        PAPP =atol(resultString);
        #endif
        }
    else if (sequenceIsOK = bufferTeleinfo[1]=='H') {
      #define Linky_BASE true
        #ifdef Linky_BASE
        HHPHC = resultString[0];
        #endif
        }
    break;

  case 10:
    if (sequenceIsOK = bufferTeleinfo[1]=='H') { 
      #define Linky_HCHP true
        #ifdef Linky_HCHP
        HHPHC = resultString[0];
        #endif
        }
    else if (sequenceIsOK = bufferTeleinfo[1]=='O') { 
      #define Linky_BASE true
        #ifdef Linky_BASE
        MOTDETAT = String(resultString);
        #endif
        }
    break;
        
  case 11:
    if (sequenceIsOK = bufferTeleinfo[1]=='O')  MOTDETAT = String(resultString);
    break;

  }
    if(!sequenceIsOK) {
      Serial.print(F("Out of sequence ..."));
      Serial.println(bufferTeleinfo);
    }
  return sequenceIsOK;
}
// ---------------------------------------------- //

// ---------------------------------------------- //
//            Checksum Calculation                //
char chksum(char *buff, uint8_t len)  {
  int i;
  char sum = 0;
  for (i=1; i<(len-2); i++) 
    sum = sum + buff[i];
  sum = (sum & 0x3F) + 0x20;
  return(sum);
}
// ---------------------------------------------- //

// ---------------------------------------------- //
//          Print TIC frame from Linky            //
void displayTeleInfo()  {  
//  Serial.print(F("ADCO ")); Serial.println(ADCO);
  Serial.print(F("OPTARIF "));  Serial.println(OPTARIF);
//  Serial.print(F("ISOUSC ")); Serial.println(ISOUSC);
  Serial.print(F("HCHC ")); Serial.println(HCHC);
  Serial.print(F("HCHP ")); Serial.println(HCHP);
  Serial.print(F("BASE ")); Serial.println(BASE);
  Serial.print(F("PTEC ")); Serial.println(PTEC);
  Serial.print(F("IINST "));Serial.println(IINST);
//  Serial.print(F("IMAX ")); Serial.println(IMAX);
  Serial.print(F("PAPP ")); Serial.println(PAPP);
//  Serial.print(F("HHPHC ")); Serial.println(HHPHC);
//  Serial.print(F("MOTDETAT ")); Serial.println(MOTDETAT);   
}
// ---------------------------------------------- //

// ---------------------------------------------- //
//      Called from setup to activate LoKy        //
void LMiC_Startup() {
  os_init();
  // Reset the MAC state.
  LMIC_reset(); LMIC_setLinkCheckMode(1); LMIC_setAdrMode(1);  
  // Increase window time for clock accuracy problem
  LMIC_setClockError(MAX_CLOCK_ERROR * 1 / 100);
  // Join the network, sending will be started after the event "Joined"
  LMIC_startJoining();
}
// ---------------------------------------------- //

// ---------------------------------------------- //
//                   LoKy setup                   //
void setup() {
  Serial.begin(9600);
  TeleInfo();    // LoKyTIC init
  Serial.println("");
  Serial.println("*-_______________________________-*");
  Serial.println(F("      .::LoKy (re)starting::."        ));
  delay(100);
//  Check_SupCapa();
  LMiC_Startup();       // LMiC init
  do_send(&sendjob);    // OTAA start
}
// ---------------------------------------------- //

//*     .::Fixed LOOP - Do NOT interfered::.     *//
void loop() {
  os_runloop_once();
  }
