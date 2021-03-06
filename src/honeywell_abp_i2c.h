/* honeywell_abp.h

Software Copyright Neil Hancock see LICENSE.md

I2C protocol https://sensing.honeywell.com/index.php?ci_id=45841
https://sps.honeywell.com/us/en/products/sensing-and-iot/sensors/pressure-sensors/board-mount-pressure-sensors/basic-abp-series

This lsimple software design has been influenced  by 
Arduino paradigm for availability and simple I2C access 
https://github.com/rodan/honeywell_hsc_ssc_i2c - for algorithims
and designed to be incorporated in
https://github.com/EnviroDIY/ModularSensors

It has been tested with 
ABPMAND001PG2A3

*/

#ifndef Honeywell_ABP_h
#define Honeywell_ABP_h

#include <Arduino.h>
#include <Wire.h>

class Honeywell_ABP {
    public:

        Honeywell_ABP(uint16_t i2c_address, float  pressure_min, float  pressure_max);
        bool getData();
        const char* getStatus() const;
        float getPressure() {return _Pressure_NoUnits;};
        float getTemperature_C() {return _Temperature_C;};
        //float OutputFunction(uint16_t output);
        //float ConversionFunction(uint16_t output);

        uint8_t IC_status=0xff;
        uint16_t IC_rawPressure;
        uint16_t IC_rawTemperature;

    private:
        uint16_t _i2c_address;

        float _pressure_min; //Client supplied Miniumium
        float _pressure_max; //Client supplied Maximium

        float _Pressure_NoUnits;
        float _Temperature_C;

        const uint16_t mOutputMin = 0x0666;
        const uint16_t mOutputMax = 0x399A;

};


#endif