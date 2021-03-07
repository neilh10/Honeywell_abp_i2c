/* nh read_sensor.cpp */

//#include <Wire.h>
#include "honeywell_abp_i2c.h"

#define SENSOR_POLL_TIME_SEC 30
const char sensorUnderTest[] = "ABPMAND001PG2A3 Range 1PSI";

#define USE_POWER_PIN
#if defined USE_POWER_PIN
const int8_t powerPin = 22;
#define PowerOn()  digitalWrite(powerPin, HIGH)
#define PowerOff()  digitalWrite(powerPin, LOW)
#endif //USE_POWER_PIN

#define USE_RTCLIB Sodaq_DS3231
#if defined USE_RTCLIB 
#include <Sodaq_DS3231.h>
USE_RTCLIB  rtcExtPhy;
#endif 

//Testeed  on ABPMAND001PG2A3
#define SLAVE_ADDR 0x28
#define OUTPUT_MIN 0x666        // 10% 16,384
#define OUTPUT_MAX 0x399A       // 90% of 2^14 - 1
#define PRESSURE_MIN 0.0        // min is 0 for sensors that give absolute values
//#define PRESSURE_MAX 206842.7   // 30psi (and we want results in pascals)
#define PRESSURE_MAX 2.3   // 1psi 2.3feet H20  ABPMAND001PG2A3

Honeywell_ABP abp_sensor(
  SLAVE_ADDR,   // I2C address
  0,      // minimum pressure
  PRESSURE_MAX);

uint32_t  counter =0;

#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
    #define SERIAL SerialUSB
#else
    #define SerialTty Serial
    #define SERIAL_PORT_USBVIRTUAL Serial
#endif

#if defined USE_RTCLIB 
//This assumes time already set and RTC in UTC, and need to adapt to local time
// The test environment is a EnviroDIY Mayfly 0.5b
int8_t _loggerTimeZone = -8;
DateTime StartUp_dt;
#define HOURS_TO_SECS 3600
#define EPOCH_TIME_OFF 946684800 

uint32_t getNowSecs2kTz(void) {
    int64_t currentEpochTime = (int64_t)((uint64_t)rtc.now().getEpoch());
    currentEpochTime += (_loggerTimeZone * HOURS_TO_SECS);
    return (uint32_t)currentEpochTime-EPOCH_TIME_OFF;
}
#endif //defined USE_RTCLIB 

void setup()
{

    // User to Enter <CR> to start
    SerialTty.begin(115200);
    while (!SerialTty.available());
    Wire.begin();

    SerialTty.print("Honeywell ABP (also HSC SCC): Poll sensor ");
    SerialTty.print(sensorUnderTest);
    SerialTty.print(" every (sec) ");
    SerialTty.println(SENSOR_POLL_TIME_SEC);

    #if defined USE_POWER_PIN
    // If power defined, use it to turn ON before access
    // Turn OFF after access.
    if (powerPin >= 0) {
        PowerOff();
        pinMode(powerPin, OUTPUT);
        SerialTty.print(F("Using power pin"));
        SerialTty.println(powerPin);
    }
    #endif //USE_POWER_PIN

    #if defined USE_RTCLIB 
    if (rtcExtPhy.begin()) {
        StartUp_dt= getNowSecs2kTz();//2000
        String dateTimeStr;
        StartUp_dt.addToString(dateTimeStr);
        SerialTty.print(F("Current RTC time is: "));
        SerialTty.println(dateTimeStr);
    }
    #endif 
} // Setup

void loop()
{
    char p_str[10], t_str[10];

    counter++;
    #if defined USE_POWER_PIN
    PowerOn();
    delay(2);//Alow for powering up of sensor 
    #endif //USE_POWER_PIN

    #if defined USE_RTCLIB 
    DateTime time_dt =getNowSecs2kTz();//2000
    String dateTimeStr;
    time_dt.addToString(dateTimeStr);
    SerialTty.print(dateTimeStr);
    SerialTty.print(", ");   
    #endif //USE_RTCLIB 
    
    SerialTty.print( counter);
    SerialTty.print(", ");   

    if (abp_sensor.getData()) {
        dtostrf(abp_sensor.getPressure(), 3, 3, p_str);
        dtostrf(abp_sensor.getTemperature_C(), 2, 2, t_str);
        SerialTty.print("depth(ft),");
        SerialTty.print(p_str);
        SerialTty.print(", temp(C),");
        SerialTty.print(t_str);

        #if 1
        SerialTty.print("  ,");
        SerialTty.print(abp_sensor.IC_status, BIN);
        SerialTty.print(",");
        SerialTty.print(abp_sensor.IC_rawPressure, DEC);
        SerialTty.print(",");
        SerialTty.print(abp_sensor.IC_rawTemperature, DEC);
        #endif

        SerialTty.println("");
    } else {
        SerialTty.print(" failed to read sensor");  
    }
    #if defined USE_POWER_PIN
    PowerOff();
    #endif //USE_POWER_PIN

   delay(SENSOR_POLL_TIME_SEC*1000);
}

