#define CCS811__CPP

#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_CCS811.h>

#include "Common.h"
#include "Sensors.h"

#include "CCS811.h"

#define DEBUG false 
#define DEBUGPRINT(x)   if(DEBUG){Serial.println(x);}

//#undef CCS811_EXISTS

namespace MeteoMega::CCS811
{
  MeteoMega::Sticker::Sticker *stickerInit;  
  MeteoMega::Sticker::Sticker *stickerCO2;
  MeteoMega::Sticker::Sticker *stickerTVOC;
  
  int16_t correctionDeltaCO2 = 0; 
  int16_t correctionDeltaTvoc = 0; 

  int pollingInterval_ms;

  uint32_t clockTimer;

  CCS811data css811data, css811dataMin, css811dataMax;

  bool sensorExists = false;

  Adafruit_CCS811 ccs;

  void InitCCS811(
      MeteoMega::Sticker::Sticker *pInitSticker,
      MeteoMega::Sticker::Sticker *pStickerCO2,
      MeteoMega::Sticker::Sticker *pStickerTVOC, int pollingInterval)
  {
    stickerInit = pInitSticker;
    stickerCO2 = pStickerCO2;
    stickerTVOC = pStickerTVOC;
#ifdef CCS811_EXISTS
    pollingInterval_ms = pollingInterval;
#else
    pollingInterval_ms = 5000;
#endif

    css811data.co2 = 0;
    css811data.tvoc = 0;

    css811dataMin.co2 = 10000;
    css811dataMin.tvoc = 10000;

    css811dataMax.co2 = 0;
    css811dataMax.tvoc = 0;

    stickerCO2->SetUnit((char *)"ppm");
    stickerTVOC->SetUnit((char *)"ppb");

    initSensor();
    clockTimer = millis();
  }

  void UpdateCCS811()
  {
    if ((millis() - clockTimer) > ((unsigned long)pollingInterval_ms))
    {
      clockTimer = millis();
      acquireCO2_TVOC();

      setCorrection();
  
      setEnvironmentCompensation();

      if(css811data.co2 < css811dataMin.co2 )
      {
        css811dataMin.co2 = css811data.co2;
        stickerCO2->SetMinMax((uint16_t)css811dataMin.co2, (uint16_t)css811dataMax.co2);
      }
      if(css811data.co2 > css811dataMax.co2 )
      {
        css811dataMax.co2 = css811data.co2;
        stickerCO2->SetMinMax((uint16_t)css811dataMin.co2, (uint16_t)css811dataMax.co2);
      }
      if(css811data.tvoc < css811dataMin.tvoc )
      {
        css811dataMin.tvoc = css811data.tvoc;
        stickerTVOC->SetMinMax((uint16_t)css811dataMin.tvoc, (uint16_t)css811dataMax.tvoc);
      }
      if(css811data.tvoc > css811dataMax.tvoc )
      {
        css811dataMax.tvoc = css811data.tvoc;
        stickerTVOC->SetMinMax((uint16_t)css811dataMin.tvoc, (uint16_t)css811dataMax.tvoc);
      }

      stickerCO2->Show_int((uint32_t)css811data.co2, (uint8_t)2);
      stickerTVOC->Show_int((uint32_t)css811data.tvoc, (uint8_t)2);
    }
  }

  void CalibrateCO2()
  {
    DEBUGPRINT(F("CalibrateCO2 is called."));
    acquireCO2_TVOC();
    correctionDeltaCO2 = css811data.co2 - NORMAL_CO2;
    correctionDeltaTvoc = css811data.tvoc - NORMAL_TVOC;
  }

  void setCorrection()
  {
    // Do not allow values less then 'normal' ones
    if ((int16_t)css811data.co2 - correctionDeltaCO2 < NORMAL_CO2 )
    {
      css811data.co2 = NORMAL_CO2;
    }
    else
    {
      css811data.co2 -= correctionDeltaCO2;
    }

    if((int16_t)css811data.tvoc -  correctionDeltaTvoc < NORMAL_TVOC)
    {
      css811data.tvoc = NORMAL_TVOC;
    }
    else
    {
      css811data.tvoc -= correctionDeltaTvoc;
    }
  }

  void initSensor()
  {
    stickerInit->Show_text((char *)"Init ...");
    DEBUGPRINT(F("CCS811 Sensor test"));

#ifdef CCS811_EXISTS
      
      DEBUGPRINT(F("CCS811 test"));
      if (!ccs.begin())
      {
          DEBUGPRINT(F("Failed to start sensor! Please check your wiring."));
          stickerInit->Show_text((char *)"FAILED");
          sensorExists = false;
      }
      else
      {
          DEBUGPRINT(F("CCS811 sensor OK!"));
          stickerInit->Show_text((char *)"OK");
          sensorExists = true;
      }
      if (sensorExists)
      {
          //calibrate temperature sensor
          while (!ccs.available());
          float temp = ccs.calculateTemperature();
          ccs.setTempOffset(temp - 25.0);
      }
#else
    stickerInit->Show_text((char *)"OK");
    sensorExists = true;
#endif
  }

  void acquireCO2_TVOC()
  {
#ifdef CCS811_EXISTS
      if( sensorExists && ccs.available())
      {
          float temp = ccs.calculateTemperature();
          if (ccs.readData())
          {
              css811data.co2 = ccs.geteCO2();
              css811data.tvoc = ccs.getTVOC();
              DEBUGPRINT(F("CO2: "));
              DEBUGPRINT(css811data.co2);
              DEBUGPRINT(F("ppm, TVOC: "));
              DEBUGPRINT(css811data.tvoc);
              DEBUGPRINT(F("ppb Temp:"));
              DEBUGPRINT(temp);
          }
          else
          {
              DEBUGPRINT(F("ERROR!"));
          }
      }
#else
    css811data.co2 = ((int)(css811data.co2 + 200)) % 2800;
    css811data.tvoc = ((int)(css811data.tvoc + 300)) % 9000;
#endif
  }

  void setEnvironmentCompensation()
  {
      if (css811dataMax.co2 > 0) // do not set compensation first time
      {
        DEBUGPRINT("setEnvironmentCompensation");
        MeteoMega::Sensors::Sensors sn = MeteoMega::Sensors::pollSensors();
        ccs.setEnvironmentalData(sn.hum, sn.tempC);
      }
  }

  int ReadCO2()
  {
    return css811data.co2;
  }

  int ReadTVOC()
  {
    return css811data.tvoc;
  }
}

#undef CCS811__CPP