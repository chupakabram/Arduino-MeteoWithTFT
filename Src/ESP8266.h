#ifndef ESP8266__H
#define ESP8266__H

#include <stdint.h>

#include "Sticker.h"

class LCDWIKI_KBV;

namespace MeteoMega::ESP8266
{
    void InitESP8266(MeteoMega::Sticker::Sticker *pStickerInit, LCDWIKI_KBV *lcd_i, uint32_t sendingInterval);
    
    void UpdateESP8266();
    
    void setupESP8266(); 
    void connectToWiFi();
    void changingMode();
    void connectTCP();
    void sendData();

    void showStatus();
};


#endif // ESP8266__H

