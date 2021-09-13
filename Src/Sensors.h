#ifndef SENSORS__H
#define SENSORS__H 

namespace MeteoMega::Sensors
{
    typedef struct _sensors__s
    {
        float   tempC;
        float   hum;
        int     pressHmm;
        int     co2ppm;
        int     tvoc;
    } Sensors;
    Sensors pollSensors();
};


#endif ///  SENSORS__H