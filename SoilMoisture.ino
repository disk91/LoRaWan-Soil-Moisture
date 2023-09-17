#include <lmic.h>
#include <hal/hal.h>
#include <SoftwareSerial.h>
#include <LowPower.h>
#include <SPI.h>
#include "config.h"
#include "aht10.h"
#include "keys.h"

const lmic_pinmap lmic_pins = {
    .nss = 10,
    .rxtx = LMIC_UNUSED_PIN,
    .rst = 4,
    .dio = {2, 6, LMIC_UNUSED_PIN},
};


void os_getArtEui (u1_t* buf) { 
  for ( int i = 0 ; i < 8 ; i++ ) buf[7-i] = APPEUI[i];
}
void os_getDevEui (u1_t* buf) { 
  for ( int i = 0 ; i < 8 ; i++ ) buf[7-i] = DEVEUI[i];
}
void os_getDevKey (u1_t* buf) {   
  memcpy_P(buf, APPKEY, 16);
}

void soft_reset() {
  asm volatile("jmp 0x00");
}


void setup() {
// put your setup code here, to run once:
  LOGINIT((9600));
  #ifdef DEBUG
   while (!Serial);
  #endif
  LOGLN((F("GO !")));
  
  // Init LoRaWAN
  os_init();    // Intialisation de la bibliothèque
  LMIC_reset();
  LMIC_setClockError(MAX_CLOCK_ERROR * 10 / 100); // agrandit la fenetre de reception
  LMIC_setAdrMode(0);  // desactive le mode ADR
  // Configure les canaux utilisables pour les communications  
  LMIC_setupChannel(0, 868100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      
  LMIC_setupChannel(1, 868300000, DR_RANGE_MAP(DR_SF12, DR_SF7B), BAND_CENTI);      
  LMIC_setupChannel(2, 868500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      
  LMIC_setupChannel(3, 867100000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      
  LMIC_setupChannel(4, 867300000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);     
  LMIC_setupChannel(5, 867500000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);     
  LMIC_setupChannel(6, 867700000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);      
  LMIC_setupChannel(7, 867900000, DR_RANGE_MAP(DR_SF12, DR_SF7),  BAND_CENTI);     
  LMIC_setupChannel(8, 868800000, DR_RANGE_MAP(DR_FSK,  DR_FSK),  BAND_MILLI);    
  // Configure la vitesse et la puissance de transmission   
  LMIC.dn2Dr = SF9; 
  LMIC_setDrTxpow(DR_SF9,8); 
  LMIC_setLinkCheckMode(0); 

  // AHT sensor
  if ( ! setupAht10() ) {
    LOGLN(("Failed to init AHT"));
    delay(5000);
    soft_reset();
  }

}



boolean canSleep = true;
void loop() {
  // put your main code here, to run repeatedly:
  static uint32_t tempsMs = 0;
  static uint16_t temps = TXPERIOD-10; 
         uint32_t start = millis();

 
  if ( temps >= TXPERIOD ) {
    uint8_t msg[4];
    temps = 0;

    // Get sensors status
    int32_t temp; uint32_t humidity;
    readSensors(&temp,&humidity);// populate temp and humidity objects with fresh data
    LOG(("Temp :")); LOGLN((temp/10.0));
    LOG(("Hygro :")); LOGLN((humidity));
    // --- Battery
    uint32_t b = getBatteryLevel();
    // --- moisture
    uint8_t sm = getSoilMoisturePercent();

    // Encode
    msg[0] = sm;
    msg[1] = (uint8_t)(b/25);   // mv = value * 25, scale 40mV
    msg[2] = (uint8_t)humidity; // scale 0..100
    msg[3] = (int8_t)(temp/5);  // scale 0.5 degrees

    // send
    LMIC_setDrTxpow(DR_SF10,8); 
    canSleep = false;
    lmic_tx_error_t err = LMIC_setTxData2(1, msg, 4, 0);
    if ( err != 0 ) {
       canSleep=true;
       LOG((F("Tx Err")));LOGLN((err));
    }
  }else {
    delay(1);
  }
  
  os_runloop_once();

  // update the running time for next message schedule
  uint32_t stop = millis();
  if ( stop > start ) tempsMs += (stop - start);
  while ( tempsMs > 1000 ) {
    temps++;        // count seconds
    tempsMs -= 1000;
  }
  if ( canSleep ) {
    LOGFLUSH(());
    LowPower.powerDown(SLEEP_8S, ADC_OFF,BOD_OFF);
    LOGINIT((9600));
    LOG(("."));
    temps += 8;
    updateHalTime(8000);
  }
  
}

// ================================================
// SENSORS
// ================================================

uint8_t getSoilMoisturePercent() {

  // get 3 values
  uint32_t soil = 0;
  for ( int i = 0 ; i < 3 ; i++ ) {
    pinMode(HYGRO_ON,OUTPUT);
    pinMode(PWM_OUT,OUTPUT);
    TCCR1A = bit(COM1A0);            // toggle OC1A on Compare Match
    TCCR1B = bit(WGM12) | bit(CS10); // CTC, scale to clock
    OCR1A = 1;
    ADMUX = _BV(REFS0) | _BV(MUX1);  // AVCC as reference voltage
    digitalWrite(HYGRO_ON,HIGH);
    delay(20);
    soil += analogRead(HYGRO_RD);
    delay(10);
    digitalWrite(HYGRO_ON,LOW);
  }
  soil /= 3;
  LOG(("Soil Raw:")); LOGLN((soil));
  // value between 900 (dry) and 500 (in water)
  if ( soil > 900 ) soil = 900;
  soil = 900 - soil;
  if ( soil > 400 ) soil = 400;
  soil /= 4; // 0..100%
  LOG(("Soil %:")); LOGLN((soil));
  return ( uint8_t) soil;
  
}

uint32_t getBatteryLevel() {
  
  analogReference(INTERNAL);
  uint32_t bat = analogRead(BATLVL_PIN);
  bat = (3333*bat)/1024;
  LOG(("Bat mV:"));LOGLN((bat));
  return bat; 
  
}

// ================================================
// LORAWAN
// ================================================

// Add in hal.cpp
// uint64_t hal_compensate_tics = 0;
// u4_t hal_ticks () {
// ...
//    return (scaled | ((uint32_t)overflow << 24)) + (hal_compensate_tics);
extern uint64_t hal_compensate_tics;
void updateHalTime(uint64_t ms) {
  // We have 62.5 tics per ms -- 1 tics = 16uS
  hal_compensate_tics += (625*ms)/10;
}

// 
void onEvent (ev_t ev) {
    LOG(("Ev : "));LOGEVENT(ev);LOGLN((""));
    switch(ev) {
        case EV_JOINED:
            LMIC_setLinkCheckMode(0);
            break;
        case EV_JOIN_TXCOMPLETE:
        case EV_JOIN_FAILED:
        case EV_TXCOMPLETE: 
        case EV_TXCANCELED:
            canSleep = true;
            break;
        case EV_TXSTART:
            canSleep = false;
        default:
            break;
    }
}
