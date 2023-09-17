// largely inspired by https://github.com/Thinary/AHT10/blob/master/src/Thinary_AHT10.h
#include <arduino.h>
#include <Wire.h>
#include "aht10.h"

Sensor_CMD eSensorCalibrateCmd[3] = {0xE1, 0x08, 0x00};
Sensor_CMD eSensorNormalCmd[3]    = {0xA8, 0x00, 0x00};
Sensor_CMD eSensorMeasureCmd[3]   = {0xAC, 0x33, 0x00};
Sensor_CMD eSensorResetCmd        = 0xBA;
Sensor_CMD eSensorInitCmd[3]      = {0xBE, 0x08, 0x00};


unsigned char readStatus(void)
{
    unsigned char result = 0;
    Wire.requestFrom(eAHT10Address_default, 1);
    result = Wire.read();
    return result;
}

bool setupAht10() {
    delay(40);

    Wire.begin(eAHT10Address_default);
    Wire.beginTransmission(eAHT10Address_default);
    Wire.write(eSensorInitCmd, 3);
    Wire.endTransmission();
    delay(100);
    uint32_t t,h;
    readSensors(&t,&h);
    delay(100);
        
    Wire.begin(eAHT10Address_default);
    Wire.beginTransmission(eAHT10Address_default);
    Wire.write(eSensorCalibrateCmd, 3);
    Wire.endTransmission();
    delay(100);
    
    return ((readStatus()&0x68) == 0x08);

}

// Returns the sensor values with temp *10
// not sure about negative value and in my case the temp is invalid
uint8_t readSensors(int32_t * tempe, uint32_t * humidity) {
    uint32_t temp[6];

    Wire.beginTransmission(eAHT10Address_default);
    Wire.write(eSensorMeasureCmd, 3);
    Wire.endTransmission();
    // 75 ms for calculation
    delay(100);

    Wire.requestFrom(eAHT10Address_default, 6);
    for(unsigned char i = 0; Wire.available() > 0; i++) {
        temp[i] = Wire.read();
    }   
    
    *humidity = ((100 * (((temp[1] << 16) | (temp[2] << 8) | temp[3]) >> 4)) / 1048576);
    *tempe = ((2000 * (((((temp[3] & 0x0F) << 16) | (temp[4] << 8) | temp[5])) & 0xFFFFFL )) / 1048576) - 500;

    return (uint8_t)temp[0];
}


void reset(void)
{
    Wire.beginTransmission(eAHT10Address_default);
    Wire.write(eSensorResetCmd);
    Wire.endTransmission();
    delay(20);
}
