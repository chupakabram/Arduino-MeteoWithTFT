#define SENSORS__CPP

#include "Bme280.h"
#include "CCS811.h"
#include "Sensors.h"

namespace MeteoMega::Sensors
{ 
    Sensors pollSensors()
    {
        Sensors s;
#if true
        s.tempC = MeteoMega::BME280::ReadTemperature();
        s.hum = MeteoMega::BME280::ReadHumidity();
        s.pressHmm = MeteoMega::BME280::ReadPressure();
        s.co2ppm = MeteoMega::CCS811::ReadCO2();
        s.tvoc = MeteoMega::CCS811::ReadTVOC();
 #else
        s.tempC = 24.0;
        s.hum = 64.0;
        s.pressHmm = 754;
        s.co2ppm = 882;
        s.tvoc = 2322; 
#endif
        return s;   
    }
};
#undef SENSORS__CPP