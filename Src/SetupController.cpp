#define SETUP_CONTROLLER__CPP

#include <Arduino.h>

#include "Common.h"
#include "Timer.h"
#include "Diagrams.h"

#include "SetupController.h"

namespace MeteoMega::SetupController
{

    typedef enum
    {
        eSetDiagrams = 0,
        eSetTimer
    } eState;

    eState state;

    void Init()
    {
        state = eSetDiagrams;
    }

    void Update()
    {
    }

    void ClickHandler()
    {
        //Serial.println("ClickHandler");
        if (eSetDiagrams == state)
        {
            MeteoMega::Diagrams::SwitchDisplayMode();
        }
        else
        {
            MeteoMega::Timer::increaseParameter(); 
        }
    }

    void DoubleClickHandler()
    {
        //Serial.println("DoubleClickHandler");
        if (eSetDiagrams == state)
        {
            state = eSetTimer;
            MeteoMega::Timer::startSetup();
        }
        else
        {
            state = eSetDiagrams;
            MeteoMega::Timer::cancelSetup();
        }
    }

    void PressHandler()
    {
        //Serial.println("PressHandler");
        if (eSetDiagrams == state)
        {
            MeteoMega::Diagrams::SwitchTimeMode();
        }
        else
        {
            if (MeteoMega::Timer::saveParameterGoToNext())
            {
                state = eSetDiagrams;
            }
        }
    }
};

#undef SETUP_CONTROLLER__CPP