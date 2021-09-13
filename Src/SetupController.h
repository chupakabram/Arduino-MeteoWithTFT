#ifndef SETUP_CONTROLLER__H
#define SETUP_CONTROLLER__H

#include <stdint.h>

namespace MeteoMega::SetupController
{
    void Init();
    void Update();


    void ClickHandler();
    void DoubleClickHandler();
    void PressHandler();

};


#endif // SETUP_CONTROLLER__H