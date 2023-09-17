// largely inspired by https://github.com/Thinary/AHT10/blob/master/src/Thinary_AHT10.h

#ifndef __AHT10_H__
#define __AHT10_H__

typedef enum {
    eAHT10Address_default = 0x38,
    eAHT10Address_Low     = 0x38,
    eAHT10Address_High    = 0x39,
} HUM_SENSOR_T;

typedef unsigned char Sensor_CMD;


bool setupAht10();
uint8_t readSensors(int32_t * tempe, uint32_t * humidity);

#endif // __AHT10_H__
