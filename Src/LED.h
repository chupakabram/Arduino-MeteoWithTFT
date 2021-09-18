#ifndef LED__H
#define LED__H

namespace MeteoMega::LED
{
    typedef enum
    {
        eNormal = 0,
        eMiddle,
        eHigh
    } eColor;

    void InitLED();
    void UpdateLED();
    
    void SetColor(eColor color_i);
    void SetBright(bool bright);

    void led(bool switchOn);
    void setBlink(bool blink_i);
    void prepareColorsAndLed();
};


#endif // LED__H