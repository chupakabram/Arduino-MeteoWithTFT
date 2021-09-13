#define LED__CPP

#include <stdint.h>
#include <Arduino.h>

#include "Common.h"
#include "LED.h"

namespace MeteoMega::LED
{
    uint8_t r,g,b;
    uint8_t colors[2][3][3]={{{0,255,0},{0,0,255},{255,0,0}},
                               {{0,128,0},{0,0,128}, {128,0,0}}};
    bool bright = true;
    uint8_t bi = 0;

    uint16_t blinkInterval = 1500; // 1.5 seconds for blinking
    uint32_t clockBlinking;

    bool blink = false;
    bool dotFlag = false;

    void InitLED()
    {
        pinMode(LED_R_PIN, OUTPUT);
        pinMode(LED_G_PIN, OUTPUT);
        pinMode(LED_B_PIN, OUTPUT);

        r = colors[bi][0][0]; 
        g = colors[bi][0][1]; 
        b = colors[bi][0][2];

        blink = false;
    }

    void UpdateLED()
    {
        if (blink)
        {
            if( (millis() - clockBlinking) > blinkInterval)
            {
                led(dotFlag);
                dotFlag = !dotFlag;
                clockBlinking = millis();
            }
        }
    }
    
    void SetColor(eColor color_i)
    {
        r = colors[bi][(uint8_t)color_i][0]; 
        g = colors[bi][(uint8_t)color_i][1]; 
        b = colors[bi][(uint8_t)color_i][2];

        led(true); 

        setBlink( eColor::eHigh == color_i);
    }

    void SetBright(bool bright_i)
    {
        bright = bright_i;
        bi = bright ? 0 : 1; 
    }

    void setBlink(bool blink_i)
    {
        blink = blink_i;
        dotFlag = true;
        if (blink)
        {
            clockBlinking = millis();
        }
        else
        {
            led(true);
        }
    }

    void led(bool switchOn)
    {
        analogWrite(LED_R_PIN, 0);
        analogWrite(LED_G_PIN, 0);
        analogWrite(LED_B_PIN, 0);
        if( switchOn)
        {
            analogWrite(LED_R_PIN, r);
            analogWrite(LED_G_PIN, g);
            analogWrite(LED_B_PIN, b);
        }
    }
}

#undef LED__CPP