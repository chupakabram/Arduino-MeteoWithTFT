#ifndef DIAGRAMS__H
#define DIAGRAMS__H

#include <stdint.h>
#include <Arduino.h>

class LCDWIKI_KBV;

namespace MeteoMega::Diagrams
{
    typedef enum
    {
        e5min = 0,
        e60min
    } eTimeMode;

    typedef enum
    {
        eTempHum = 0,
        eTempPress,
        eTempCO2,
        eTempTVOC
    } eDisplayMode;

    // typedef struct _recordMeteo_t
    // {
    //     int16_t     tempC;             // degrees * 10 ; 292 means 29.2
    //     int16_t     hum;
    //     int16_t     pressHmm;
    //     int16_t     co2ppm;
    //     int16_t     tvoc;
    // } recordMeteo;

    void InitDiagrams(LCDWIKI_KBV *lcd_i, uint16_t x, uint16_t y, uint16_t dx, uint16_t dy, eTimeMode = e5min);
   
	void SetBackgroundColor(uint16_t colour_i);
	void SetFrameColor(uint16_t colour_i);

    void SetTemperatureColor(uint16_t colour_i);
    void SetHumidityColor(uint16_t colour_i);
    void SetPressureColor(uint16_t colour_i);
    void SetCO2Color(uint16_t colour_i);
    void SetTVOCColor(uint16_t colour_i);

    void SwitchTimeMode();
    void SwitchDisplayMode();

    void UpdateDiagrams();

    void prepareData(eTimeMode mode_i);

    //void draw();
    bool redraw();

    void findMinMax();
    void findMinMaxLabels();
    void drawFrame();

    bool show(int16_t param [], int16_t cf, int16_t gradMin, int16_t gradMax, float scale, uint16_t color, int16_t x_offset);

    // void showTemperature();
    // void showHumidity();
    // void showPressure();
    // void showCO2();
    // void showTVOC();
};


#endif // DIAGRAMS__H