#include <avr/pgmspace.h>
#include "config.h"

#ifdef DEBUG
const char st_00[] PROGMEM = "          ";
const char st_01[] PROGMEM = "scan_tmout";
const char st_02[] PROGMEM = "beacon_fnd";
const char st_03[] PROGMEM = "beacon_mis";
const char st_04[] PROGMEM = "beacon_trk";
const char st_05[] PROGMEM = "joining   ";
const char st_06[] PROGMEM = "joined    ";
const char st_07[] PROGMEM = "rfu1      ";
const char st_08[] PROGMEM = "join_fail ";
const char st_09[] PROGMEM = "rejoin_fld";
const char st_10[] PROGMEM = "tx_complet";
const char st_11[] PROGMEM = "lost_tsync";
const char st_12[] PROGMEM = "reset     ";
const char st_13[] PROGMEM = "rx_complet";
const char st_14[] PROGMEM = "link_dead ";
const char st_15[] PROGMEM = "link_alive";
const char st_16[] PROGMEM = "scan_found";
const char st_17[] PROGMEM = "tx_start  ";
const char st_18[] PROGMEM = "tx_cancel ";
const char st_19[] PROGMEM = "rx_start  ";
const char st_20[] PROGMEM = "join_tx_ok";


const char *const eventToText[] PROGMEM = {
  // EV_SCAN_TIMEOUT=1, EV_BEACON_FOUND,
  // EV_BEACON_MISSED, EV_BEACON_TRACKED, EV_JOINING,
  // EV_JOINED, EV_RFU1, EV_JOIN_FAILED, EV_REJOIN_FAILED,
  // EV_TXCOMPLETE, EV_LOST_TSYNC, EV_RESET,
  // EV_RXCOMPLETE, EV_LINK_DEAD, EV_LINK_ALIVE, EV_SCAN_FOUND,
  // EV_TXSTART, EV_TXCANCELED, EV_RXSTART, EV_JOIN_TXCOMPLETE
  st_00, st_01, st_02, st_03, st_04, st_05, st_06, st_07, st_08, st_09, 
  st_10, st_11, st_12, st_13, st_14, st_15, st_16, st_17, st_18, st_19, 
  st_20   
};
# else
const char *const eventToText[] PROGMEM = { "" };
# endif // DEBUG
