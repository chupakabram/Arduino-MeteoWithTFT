#ifndef PHOTORESISTOR__H
#define PHOTORESISTOR__H

#include <stdint.h>

namespace MeteoMega::PhotoResistor
{
    void InitPhotoresistor();
    void UpdatePhotoresistor();

    bool IsDark();
};


#endif // PHOTORESISTOR__H