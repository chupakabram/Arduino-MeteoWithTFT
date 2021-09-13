#ifndef TIMER__H
#define TIMER__H 

#include <stdint.h>

#include "Sticker.h"

class LCDWIKI_KBV;

namespace MeteoMega::Timer
{
    typedef struct _time__s
    {
        int hour;
        int min;
        int sec;
        int month;
        int day;
        int year;
        int dayOfWeek;
    } Time;

    void InitTimer(MeteoMega::Sticker::Sticker * pInitSticker, MeteoMega::Sticker::Sticker * pSticker, int pollingInterval_ms);
    void UpdateTimer();

    Time ReadTime();

    void initDS3231();
    void acquireTime();

    void drawFirstTime();

    void drawWithSticker();
    void draw();

    void drawSetupMode();

    void startSetup();
    void cancelSetup();
    void increaseParameter();
    bool saveParameterGoToNext();

    void prepareStringToDraw(uint8_t *arr, int val);
    void drawParameter(uint8_t *arr, uint16_t x, uint16_t y, uint16_t size, uint16_t backColor);
};

#endif ///  TIMER__H