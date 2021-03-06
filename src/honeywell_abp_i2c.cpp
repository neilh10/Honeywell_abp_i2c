/* honeywell_abp_i2c.cpp
*/

#include "honeywell_abp_i2c.h"

Honeywell_ABP::Honeywell_ABP(uint16_t i2c_address,  float pressure_min, float  pressure_max)
{
    _i2c_address = i2c_address;
    _pressure_min =  pressure_min;
    _pressure_max =  pressure_max;
}

#define I2C_SESNOR_BYTES 4
bool Honeywell_ABP::getData()
{
    uint8_t wire_buf[I2C_SESNOR_BYTES] = { 0, 0, 0, 0 };
    int bytes_rx = Wire.requestFrom(_i2c_address, (uint8_t) I2C_SESNOR_BYTES);
    IC_status=0xff;
    if( bytes_rx == I2C_SESNOR_BYTES) // Did we receive the number of requested bytes?
    {
        for(int i = 0; i < I2C_SESNOR_BYTES; i++)
        {
            wire_buf[i] = Wire.read();
        }
        IC_status = wire_buf[0] >> 6;
        IC_rawPressure = (wire_buf[0] << 8 | wire_buf[1]) & 0x3FFF;
        IC_rawTemperature = ((wire_buf[2] << 8) | (wire_buf[3])) >> 5;

        uint16_t raw_sensor = IC_rawPressure;
        if (raw_sensor  < mOutputMin) { 
            raw_sensor  = mOutputMin;
        }else if (raw_sensor  > mOutputMax) {
            raw_sensor  = mOutputMax;
        }
        _Pressure_NoUnits =  (float)(raw_sensor - mOutputMin) * (float)(_pressure_max - _pressure_min) / (float)(mOutputMax - mOutputMin) + _pressure_min;        

        _Temperature_C = (IC_rawTemperature * 0.0977) - 50;

        return true;
    }

    return false;
}

const char* Honeywell_ABP::getStatus() const
{
    switch(IC_status)
    {
        case 0: return "Status Code 0: Normal operation.";
        case 1: return "Status Code 1: Device in command mode";
        case 2: return "Status Code 2: Stale data.";
        case 3: return "Status Code 3: Diagnostic condition.";
        default: return "Unknown error";
    }
}