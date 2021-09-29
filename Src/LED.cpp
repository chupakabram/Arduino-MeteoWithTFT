#define LED__CPP

#include <stdint.h>
#include <Arduino.h>

#include "Common.h"
#include "LED.h"

#define DEBUG false 
#define DEBUGPRINT(x)   if(DEBUG){Serial.println(x);}

namespace MeteoMega::LED
{
    uint8_t r,g,b;
    uint8_t colors[2][3][3]={ {{0,96,0}, {64,64,0},  {96,0,0}},
                              {{0,48,0}, {32,32,0,}, {48,0,0}}  };
  
    bool bright = true;
    uint8_t bi = 0;

    uint16_t blinkInterval = 1500; // 1.5 seconds for blinking
    uint32_t clockBlinking;

    bool blink = false;
    bool dotFlag = false;
    eColor color = eColor::eNormal;

    void InitLED()
    {
        pinMode(LED_R_PIN, OUTPUT);
        pinMode(LED_G_PIN, OUTPUT);
        pinMode(LED_B_PIN, OUTPUT);

        blink = false;

        r = colors[bi][0][0]; 
        g = colors[bi][0][1]; 
        b = colors[bi][0][2];

            DEBUGPRINT(r)
            DEBUGPRINT(g)
            DEBUGPRINT(b)
            DEBUGPRINT("-------")
        
        prepareColorsAndLed();
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
        else
        {
            DEBUGPRINT(r)
            DEBUGPRINT(g)
            DEBUGPRINT(b)
            DEBUGPRINT("-------")
            //led(true);
        }
    }

    void SetColor(eColor color_i)
    {
        if (color_i != color)
        {
            color = color_i;
            setBlink(eColor::eHigh == color_i);
            prepareColorsAndLed();
        }
    }

    void SetBright(bool bright_i)
    {
        if(bright_i != bright)
        {
            bright = bright_i;
            bi = bright ? 0 : 1; 
            prepareColorsAndLed();
        }
    }

    void setBlink(bool blink_i)
    {
        if (blink != blink_i)
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
    }

    void prepareColorsAndLed()
    {
        r = colors[bi][(uint8_t)color][0];
        g = colors[bi][(uint8_t)color][1];
        b = colors[bi][(uint8_t)color][2];
        led(true);
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