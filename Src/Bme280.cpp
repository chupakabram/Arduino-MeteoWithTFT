#define BME280__CPP

#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include "Bme280.h"

#define DEBUG false 
#define DEBUGPRINT(x)   if(DEBUG){Serial.println(x);}

//#undef BME280_EXISTS

namespace MeteoMega::BME280
{
  MeteoMega::Sticker::Sticker *stickerInit; 
  MeteoMega::Sticker::Sticker *stickerTemperature;
  MeteoMega::Sticker::Sticker *stickerHumidity;
  MeteoMega::Sticker::Sticker *stickerPressure;
  int pollingInterval_ms;

  uint32_t clockTimer;

  BME280data bme289data, bme289dataMin, bme289dataMax;

  bool sensorExists = false;

  Adafruit_BME280 bme; // use I2C interface

  void InitBME280(
      MeteoMega::Sticker::Sticker * pInitSticker,
      MeteoMega::Sticker::Sticker *pStickerHumidity,
      MeteoMega::Sticker::Sticker *pStickerTemperature,
      MeteoMega::Sticker::Sticker *pStickerPressure, int pollingInterval)
  {
    stickerInit = pInitSticker;
    stickerTemperature = pStickerTemperature;
    stickerHumidity = pStickerHumidity;
    stickerPressure = pStickerPressure;
#ifdef BME280_EXISTS
    pollingInterval_ms = pollingInterval;
#else
    pollingInterval_ms = 7000;
#endif

    bme289data.tempC = 0;
    bme289data.hum = 0;
    bme289data.pressHmm = 0;

    bme289dataMin.tempC = 300;
    bme289dataMin.hum = 200;
    bme289dataMin.pressHmm = 1000;

    bme289dataMax.tempC = -300;
    bme289dataMax.hum = -200;
    bme289dataMax.pressHmm = -1000;

    initSensor();
    clockTimer = millis();

    pStickerTemperature->SetUnit((char *) "\366C");
    pStickerHumidity->SetUnit((char*)"%");
    pStickerPressure->SetUnit((char *)"Hmm");
  }

  void UpdateBME280()
  {
    if ((millis() - clockTimer) > ((unsigned long)pollingInterval_ms))
    {
      clockTimer = millis();
      acquirePressureHumidityTemperature();

      if(bme289data.tempC < bme289dataMin.tempC )
      {
        bme289dataMin.tempC = bme289data.tempC;
        stickerTemperature->SetMinMax(bme289dataMin.tempC, bme289dataMax.tempC);
      }
      if(bme289data.tempC > bme289dataMax.tempC )
      {
        bme289dataMax.tempC = bme289data.tempC;
        stickerTemperature->SetMinMax(bme289dataMin.tempC, bme289dataMax.tempC);
      }

      if(bme289data.hum < bme289dataMin.hum )
      {
        bme289dataMin.hum = bme289data.hum;
        stickerHumidity->SetMinMax((uint16_t)bme289dataMin.hum, (uint16_t)bme289dataMax.hum);
      }
      if(bme289data.hum > bme289dataMax.hum )
      {
        bme289dataMax.hum = bme289data.hum;
        stickerHumidity->SetMinMax((uint16_t)bme289dataMin.hum, (uint16_t)bme289dataMax.hum);
      }

      if(bme289data.pressHmm < bme289dataMin.pressHmm )
      {
        bme289dataMin.pressHmm = bme289data.pressHmm;
        stickerPressure->SetMinMax((uint16_t)bme289dataMin.pressHmm, (uint16_t)bme289dataMax.pressHmm);
      }
      if(bme289data.pressHmm > bme289dataMax.pressHmm )
      {
        bme289dataMax.pressHmm = bme289data.pressHmm;
        stickerPressure->SetMinMax((uint16_t)bme289dataMin.pressHmm, (uint16_t)bme289dataMax.pressHmm);
      }

      stickerTemperature->Show_float((float)bme289data.tempC, (uint8_t)2, (uint8_t)1);
      stickerHumidity->Show_int((uint32_t)bme289data.hum, (uint8_t)2);
      stickerPressure->Show_int((uint32_t)bme289data.pressHmm, (uint8_t)2);
    }
  }
  void initSensor()
  {
    stickerInit->Show_text((char *)"Init ...");
    DEBUGPRINT(F("BME280 Sensor test"));

#ifdef BME280_EXISTS
    if (!bme.begin(0x76))  // !bme.begin(&Wire)
    {
      DEBUGPRINT(F("Could not find BME280 sensor !"));
      stickerInit->Show_text((char *)"FAILED");
    }
    else
    {
      DEBUGPRINT(F("BME280 sensor OK!"));
      stickerInit->Show_text((char *)"OK");
      sensorExists = true;
    }

    bme.setSampling(Adafruit_BME280::MODE_FORCED,
                    Adafruit_BME280::SAMPLING_X1, // temperature
                    Adafruit_BME280::SAMPLING_X1, // pressure
                    Adafruit_BME280::SAMPLING_X1, // humidity
                    Adafruit_BME280::FILTER_OFF);
#else
    stickerInit->Show_text((char *)"OK");
    sensorExists = true;
#endif
  }
  
  void acquirePressureHumidityTemperature()
  {
#ifdef BME280_EXISTS
    DEBUGPRINT(" ");
    DEBUGPRINT("acquirePressureHumidityTemperature");

    if (sensorExists)
    {
      bme.takeForcedMeasurement();
      bme289data.tempC = bme.readTemperature();
      bme289data.hum = (int)bme.readHumidity();
      bme289data.pressHmm = (int)((float)bme.readPressure() * 0.00750062);
    }
#else
    bme289data.tempC = ((int)(bme289data.tempC + 3)) % 39 + 0.1;
    bme289data.hum = ((int)(bme289data.hum + 10)) % 100 + 0.3;
    bme289data.pressHmm = ((int)(bme289data.pressHmm + 5)) % 100 + 700.0;
#endif
  }
  
  float ReadTemperature()
  {
    return bme289data.tempC;
  }

  float ReadHumidity()
  {
    return bme289data.hum;
  }

  int ReadPressure()
  {
    return bme289data.pressHmm;
  }
}

#undef BME280__CPP