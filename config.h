#ifndef __CONFIG_H__
#define __CONFIG_H__

#define TXPERIOD  (30*60) // 30 minutes
#define BATLVL_PIN A3
#define HYGRO_ON 5
#define PWM_OUT  9
#define HYGRO_RD A2

extern const char *const eventToText[];

//#define DEBUG
#ifdef DEBUG
  #define LOGLN(x)  Serial.println x
  #define LOG(x) Serial.print x
  #define LOGINIT(x) Serial.begin x
  #define LOGFLUSH(x) Serial.flush x
  #define LOGEVENT(x) {   \
                        char buff[11]; \
                        strcpy_P(buff, (char*)pgm_read_word(&(eventToText[x]))); \
                        Serial.print(buff); \
                      }
#else
  #define LOGLN(x) 
  #define LOG(x)
  #define LOGINIT(x)
  #define LOGFLUSH(x) 
  #define LOGEVENT(x)  
#endif

#endif // __CONFIG_H__
