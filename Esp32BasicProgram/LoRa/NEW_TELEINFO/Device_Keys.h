/* Device ID : node-1             *
/  Application ID : loky-test-1   */

static const u1_t PROGMEM APPEUI[8] = { // LSB format
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
void os_getArtEui (u1_t* buf) {memcpy_P(buf, APPEUI, 8);}

static const u1_t PROGMEM DEVEUI[8] = { // LSB format
0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX
};
void os_getDevEui (u1_t* buf) {memcpy_P(buf, DEVEUI, 8);}

static const u1_t PROGMEM APPKEY[16] = { // MSB format
0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX
};
void os_getDevKey (u1_t* buf) {memcpy_P(buf, APPKEY,16);}

// Pin setup RFM95W
const lmic_pinmap lmic_pins = {
  .nss = 10,
  .rxtx = LMIC_UNUSED_PIN,
  .rst = 8,
  .dio = {2, 7, 9},
  };
