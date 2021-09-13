#define PHOTORESISTOR__CPP

#include <Arduino.h>

#include "Common.h"

#include "LED.h"
#include "PhotoResistor.h"

#define BRIGHT_THRESHOLD_DARK   150  // threshold to switch to dark (0-1023)
#define BRIGHT_THRESHOLD_LIGHT  180  // threshold to switch to light (0-1023)

namespace MeteoMega::PhotoResistor
{
    bool isDark;

    void InitPhotoresistor()
    {
        pinMode(PHOTORESISTOR_PIN, INPUT);
        isDark = false;
    }

    void UpdatePhotoresistor()
    {
    }

    bool IsDark()
    {
        if (isDark)
        {
            if (analogRead(PHOTORESISTOR_PIN) > BRIGHT_THRESHOLD_LIGHT)
            { // it was dark, but now is light
                isDark = false;
            }
        }
        else if (analogRead(PHOTORESISTOR_PIN) < BRIGHT_THRESHOLD_DARK)
        { // it was light, but now dark
            isDark = true;
        }
        return isDark;
    }
};


#undef PHOTORESISTOR__CPP