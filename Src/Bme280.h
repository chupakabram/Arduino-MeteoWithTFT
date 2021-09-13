#ifndef BME280__H
#define BME280__H

#include <stdint.h>

#include "Sticker.h"

namespace MeteoMega::BME280
{
    typedef struct _bme280data__s
    {
        float tempC;
        float hum;
        float pressHmm;
    } BME280data;

    void InitBME280(
        MeteoMega::Sticker::Sticker * pInitSticker,
        MeteoMega::Sticker::Sticker *pStickerHumidity,
        MeteoMega::Sticker::Sticker *pStickerTemperature,
        MeteoMega::Sticker::Sticker *pStickerPressure, int pollingInterval);
    
    void UpdateBME280();
    
    float ReadTemperature();
    float ReadHumidity();
    int ReadPressure();

    void initSensor();
    void acquirePressureHumidityTemperature();
};

#endif // BME280__H