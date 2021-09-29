#ifndef CCS811__H
#define CCS811__H

#include <stdint.h>

#include "Sticker.h"

namespace MeteoMega::CCS811
{
    typedef struct _ccs811data__s
    {
        uint16_t co2;
        uint16_t tvoc;
    } CCS811data;

    void InitCCS811(
        MeteoMega::Sticker::Sticker * pInitSticker,
        MeteoMega::Sticker::Sticker *pStickerCO2,
        MeteoMega::Sticker::Sticker *pStickerTVOC, int pollingInterval);
    
    void UpdateCCS811();

    void CalibrateCO2();
    
    int ReadCO2();
    int ReadTVOC();

    void initSensor();
    void acquireCO2_TVOC();
    void setEnvironmentCompensation();
    void setCorrection();
};

#endif // CCS811__H