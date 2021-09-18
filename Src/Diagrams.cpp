#define DIAGRAMS__CPP

#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library

#include "Common.h"
#include "Sensors.h"

#include "Diagrams.h"

#define ELEMENTS 432

namespace MeteoMega::Diagrams
{
    typedef enum {
        eIdle = 0,
        eCalculate,
        eDraw
    } eState;

    LCDWIKI_KBV * _lcd;
    unsigned long _updateInterval = 30000;  // 30 seconds
    uint16_t _counter;
    unsigned long clockTimer0;
    eTimeMode timeMode;
    eDisplayMode displayMode;
	
    uint16_t yTitle;

    uint16_t x0; //position
	uint16_t y0;
	uint16_t dx; //dimension
	uint16_t dy;

    // colours
	uint16_t colour_frame;
	uint16_t colour_back;

    // curves colors
    uint16_t color_tempe;
	uint16_t color_humid;
    uint16_t color_press;
	uint16_t color_co2;
    uint16_t color_tvoc;
	
    // picture and scaling
    int16_t min_level_y;        // y-coordinate offset to print min value
    int16_t max_level_y = 15;   // y-ccordinate offsrt to print max value
    int16_t dy_active;
    int16_t centerY;

    int16_t grad_min_T, grad_max_T, grad_min_H, grad_max_H;
    int16_t grad_min_P, grad_max_P, grad_min_C, grad_max_C, grad_min_Tv, grad_max_Tv;
    float scale_T, scale_H, scale_P, scale_C, scale_Tv;

    int16_t iig;
    int16_t xs,ys,xe,ye;
    int16_t gradAv;

    int8_t drawStatus;

    // arrays to keep data history
    int16_t h_tempC[ELEMENTS];
    int16_t h_hum[ELEMENTS];
    int16_t h_pressHmm[ELEMENTS]; //36 hours, 5 min per pixel; or 18 day, 60 min per pixel
    int16_t h_co2ppm[ELEMENTS];
    int16_t h_tvoc[ELEMENTS];

    int16_t emptyValue = 0x8000;

    uint16_t _lastIdx;

    int16_t tempCMin, tempCMax;
    int16_t humMin, humMax;
    int16_t pressHmmMin, pressHmmMax;
    int16_t co2ppmMin, co2ppmMax;
    int16_t tvocMin, tvocMax;

    String period;
    String temperature = "Temperature ";

    eState  state;

    //dataRecord _historyPerDays[30];
    //dataRecord _historyPer15Min[96];

    void InitDiagrams(LCDWIKI_KBV *lcd_i, uint16_t x, uint16_t y, uint16_t dx, uint16_t dy, eTimeMode mode_i)
    {
        _lcd = lcd_i;
        
        MeteoMega::Diagrams::yTitle = y;
        MeteoMega::Diagrams::x0 = x;
        MeteoMega::Diagrams::y0 = y+10;
        MeteoMega::Diagrams::dx = dx;
        MeteoMega::Diagrams::dy = dy-10;
        
        displayMode = eDisplayMode::eTempHum;

        timeMode = mode_i;
        _counter = (uint16_t)((timeMode == eTimeMode::e5min) ? 10 : 120);
        
        _lastIdx = 0;
        for(int ii=0; ii<ELEMENTS; ++ii)
        {
            h_tempC[ii] = emptyValue;
            h_hum[ii] = emptyValue;
            h_pressHmm[ii] = emptyValue;
            h_co2ppm[ii] = emptyValue;
            h_tvoc[ii] = emptyValue;
        }

        min_level_y = ((dy - max_level_y)/32) * 32 + max_level_y;
        dy_active = min_level_y - max_level_y;
        centerY = y0 + max_level_y + dy_active/2;

        drawFrame();
        clockTimer0 = millis();

        state = eState::eIdle;

        iig = -1;
        drawStatus = 0;

        //_lcd->Set_Text_Size(1);
        //_lcd->Set_Text_colour(WHITE);
        //_lcd->Print_Number_Int(_updateInterval, 50, 150, 6, ' ', 10);

        // _lcd->Set_Text_Size(1);
        // _lcd->Set_Text_colour(WHITE);
        // uint8_t a[65];
        // for (uint8_t i = 0; i<64;++i) a[i] = (i+26)%26 +177;
        // a[64] = '\0';
        // _lcd->Print(a, 40, 140);
    }
    
    void SetFrameColor(uint16_t colour_i)
    {
        colour_frame = colour_i;
    }
    void SetBackgroundColor(uint16_t colour_i)
    {
        colour_back = colour_i;
    }

    void SetTemperatureColor(uint16_t colour_i) { color_tempe = colour_i;};
    void SetHumidityColor(uint16_t colour_i) { color_humid = colour_i;};
    void SetPressureColor(uint16_t colour_i) { color_press = colour_i;};
    void SetCO2Color(uint16_t colour_i) { color_co2 = colour_i;};
    void SetTVOCColor(uint16_t colour_i) { color_tvoc = colour_i;};

    void SwitchTimeMode()
    {
        timeMode = eTimeMode::e5min == timeMode ? eTimeMode::e60min : eTimeMode::e5min;

        prepareData(timeMode);
        _counter = (uint16_t)((timeMode == eTimeMode::e5min) ? 10 : 60);
        state = eState::eCalculate; // to make redraw next update
    }

    void SwitchDisplayMode()
    {
        displayMode = (eDisplayMode) (( (int)displayMode + 1 ) % ((int) eTempTVOC + 1));
        state = eState::eCalculate;  // to make redraw next update
    }

    void UpdateDiagrams()
    {
        if (state == eState::eIdle)
        {
            if ((millis() - clockTimer0) > ((unsigned long)_updateInterval))
            {
                clockTimer0 = millis();
                --_counter;
                if( 0 == _counter)
                {
                    state = eState::eCalculate;
                    //_lcd->Print_Number_Int(_lastIdx, 240, 150, 3, ' ', 10);
                }
            }
        }
        else if (state == eState::eCalculate)
        {

            //_lcd->Set_Text_Size(1);
            //_lcd->Set_Text_Back_colour(colour_back);
            //_lcd->Set_Text_colour(WHITE);
            //_lcd->Print_Number_Int(_lastIdx, 120, 150, 3, ' ', 10);

            Sensors::Sensors sn = Sensors::pollSensors();

            _lastIdx = (0 == _lastIdx) ? ELEMENTS - 1 : _lastIdx - 1;

            h_tempC[_lastIdx]     = tempCMin      = tempCMax      = (int16_t)(sn.tempC * 10);
            h_hum[_lastIdx]       = humMin        = humMax        = (int16_t)(sn.hum);
            h_pressHmm[_lastIdx]  = pressHmmMin   = pressHmmMax   = (uint16_t)(sn.pressHmm);
            h_co2ppm[_lastIdx]    = co2ppmMin     = co2ppmMax     = (uint16_t)(sn.co2ppm);
            h_tvoc[_lastIdx]      = tvocMin       = tvocMax       = (uint16_t)(sn.tvoc);

            findMinMax();
            findMinMaxLabels();

            state = eState::eDraw;
        }
        else if  (state == eState::eDraw)
        {
            if (redraw() )
            {
                state = eState::eIdle;
                clockTimer0 = millis();
                _counter = (uint16_t)((timeMode == eTimeMode::e5min) ? 10 : 60);
            }
        }
    }

    void prepareData(eTimeMode mode_i)
    {
        if(mode_i == e5min)
        {
            // Switch to 5 minutes interval - clear data
            _lastIdx = 0;
            for(int ii=0; ii<ELEMENTS; ++ii)
            {
                h_tempC[ii] = emptyValue;
                h_hum[ii] = emptyValue;
                h_pressHmm[ii] = emptyValue;
                h_co2ppm[ii] = emptyValue;
                h_tvoc[ii] = emptyValue;
            }
        }
        else
        {
            // Switch to 60 minutes interval - use each 12 data
            uint64_t ii = (_lastIdx + 1) % ELEMENTS;
            for(int jj=0; jj<(ELEMENTS/12); ++jj)
            {
                h_tempC[ii] = h_tempC[(ii+12) % ELEMENTS];
                h_hum[ii] = h_hum[(ii+12) % ELEMENTS];
                h_pressHmm[ii] = h_pressHmm[(ii+12) % ELEMENTS];
                h_co2ppm[ii] = h_co2ppm[(ii+12) % ELEMENTS];
                h_tvoc[ii] = h_tvoc[(ii+12) % ELEMENTS];
                ii = (ii + 1) % ELEMENTS;
            } 
            // Clear data
            while (ii != _lastIdx && h_tvoc[ii] != emptyValue)  
            {
                h_tempC[ii] = emptyValue;
                h_hum[ii] = emptyValue;
                h_pressHmm[ii] = emptyValue;
                h_co2ppm[ii] = emptyValue;
                h_tvoc[ii] = emptyValue;
                ii = (ii + 1) % ELEMENTS;       
            }         
        }
    }


#define ASSIGN_MIN(a,b) (a = (a<b)? (a) : (b))
#define ASSIGN_MAX(a,b) (a = (a>b)? (a) : (b))   

    void findMinMax()
    {
        uint16_t ii = (_lastIdx + 1) % ELEMENTS;
        while (ii != _lastIdx && h_tvoc[ii] != emptyValue)
        {
            ASSIGN_MIN(tempCMin, h_tempC[ii]);
            ASSIGN_MAX(tempCMax, h_tempC[ii]);

            ASSIGN_MIN(humMin, h_hum[ii]);
            ASSIGN_MAX(humMax, h_hum[ii]);

            ASSIGN_MIN(pressHmmMin, h_pressHmm[ii]);
            ASSIGN_MAX(pressHmmMax, h_pressHmm[ii]);

            ASSIGN_MIN(co2ppmMin, h_co2ppm[ii]);
            ASSIGN_MAX(co2ppmMax, h_co2ppm[ii]);

            ASSIGN_MIN(tvocMin, h_tvoc[ii]);
            ASSIGN_MAX(tvocMax, h_tvoc[ii]);

            ii = (ii + 1) % ELEMENTS;
        }
    }

    void findMinMaxLabels()
    {
        grad_min_T = -10+(tempCMin/100)*10; 
	    grad_max_T =  10+(tempCMax/100)*10;
    	scale_T = (float) (grad_max_T - grad_min_T)/(dy_active); 
	    if (scale_T==0) {scale_T=1;}

        grad_min_P = -10+(pressHmmMin/10)*10; 
	    grad_max_P =  10+(pressHmmMax/10)*10;
    	scale_P = (float) (grad_max_P - grad_min_P)/(dy_active); 
	    if (scale_P==0) {scale_P=1;}

        grad_min_H = -10+(humMin/10)*10; 
	    grad_max_H =  10+(humMax/10)*10;
    	scale_H = (float) (grad_max_H - grad_min_H)/(dy_active); 
	    if (scale_H==0) {scale_H=1;}

        grad_min_C = -100+(co2ppmMin/100)*100; 
	    grad_max_C =  100+(co2ppmMax/100)*100;
    	scale_C = (float) (grad_max_C - grad_min_C)/(dy_active); 
	    if (scale_C==0) {scale_C=1;}

        grad_min_Tv = -100+(tvocMin/100)*100; 
	    grad_max_Tv =  100+(tvocMax/100)*100;
    	scale_Tv = (float) (grad_max_Tv - grad_min_Tv)/(dy_active); 
	    if (scale_Tv==0) {scale_Tv=1;}
    }

    void drawFrame()
    {
        _lcd->Set_Draw_color(colour_back);
        _lcd->Fill_Rectangle(x0, yTitle, x0 + dx, yTitle + dy + 10);
        _lcd->Set_Draw_color(colour_frame);
        _lcd->Draw_Rectangle(x0 + 1, yTitle + 1, x0 + dx - 2, yTitle + dy + 10 - 2);
        _lcd->Draw_Rectangle(x0 + 1, yTitle + 1, x0 + dx - 2, yTitle + 10 - 2);

        // horizontal lines
        _lcd->Set_Draw_color(GRAY_DARK);
        for (int16_t yi = y0 + max_level_y; yi < ((int16_t)(y0+dy)); yi += 32)
        {
            _lcd->Draw_Line(x0 + 12, yi, x0 + dx - 12, yi);
        }

        // vertical lines
        for (int xi = x0 + dx - 20, i = 1; xi > 12; xi -= 12) // 1 trait de gradation VERTical tous les 12 px
        {
            _lcd->Set_Draw_color(((i % 12) == 0) ? GRAY_LIGHT : GRAY_DARK);
            _lcd->Draw_Line(xi - 1, y0 + 12, xi - 1, y0 + dy - 12);
            i++;
        }

        // time labels
        _lcd->Set_Text_Size(1);
        _lcd->Set_Text_Back_colour(colour_back);
        _lcd->Set_Text_colour(WHITE);
        if ( eTimeMode::e60min == timeMode)
        {
            _lcd->Print_String("-6d", x0 + 312, y0 + dy - 20);
            _lcd->Print_String("-12d", x0 + 168, y0 + dy - 20);
            _lcd->Print_String("-18d", x0 + 24, y0 + dy - 20);
            period = "1 hour ";
        }
        else
        {
            _lcd->Print_String("-12h", x0 + 312, y0 + dy - 20);
            _lcd->Print_String("-24h", x0 + 168, y0 + dy - 20);
            _lcd->Print_String("-36h", x0 + 24, y0 + dy - 20);
            period = "5 min ";
        }

        switch(displayMode)
        {
            case eDisplayMode::eTempHum :   _lcd->Print_String(temperature + "+ Humidity \261 " + period,    x0+32, yTitle + 5); break;
            case eDisplayMode::eTempPress : _lcd->Print_String(temperature + "+ Pressure \261 " + period,    x0+32, yTitle + 5); break;
            case eDisplayMode::eTempCO2 :   _lcd->Print_String(temperature + "+ CO2 \261 " + period,         x0+32, yTitle + 5); break;
            case eDisplayMode::eTempTVOC :  _lcd->Print_String(temperature + "+ TVOC \261 " + period,        x0+32, yTitle + 5); break;
        }
    }

    bool show(int16_t param [], int16_t cf, int16_t gradMin, int16_t gradMax, float scale, uint16_t color, int16_t x_offset)
    {
        bool retval = false;
        if (iig < 0)
        {
            iig = (_lastIdx + 1) % ELEMENTS;
            gradAv = (gradMax + gradMin) / 2;

            xs = x0 + dx - 20;
            ys = (gradAv - param[_lastIdx]/cf)/scale + centerY;

            _lcd->Set_Text_Size(1);
            _lcd->Set_Text_Back_colour(colour_back);
            _lcd->Set_Text_colour(color);
            _lcd->Print_Number_Int(gradMax, x0 + x_offset, y0 + max_level_y, 3, ' ', 10);
            _lcd->Print_Number_Int(gradMin, x0 + x_offset, y0 + min_level_y, 3, ' ', 10);

            //Serial.println(gradMax);
            //Serial.println(gradMin);
            //Serial.println(scale);
            //Serial.println(centerY);
            //Serial.println(" ");
            //Serial.println(param[_lastIdx]);
            //Serial.println(ys);
        }
        else if (iig != ((int16_t)_lastIdx) && h_tvoc[iig] != emptyValue)
        {
            xe = xs - 1;
            ye = (gradAv - param[iig]/cf) / scale + centerY;

            //Serial.println(param[iig]);
            //Serial.println(ye);

            _lcd->Set_Draw_color(color);
            _lcd->Draw_Line(xs, ys, xe, ye);
            xs = xe;
            ys = ye;
            iig = (iig + 1) % ELEMENTS;
        }
        else
        {
            //Serial.println("----------------------------");
            iig = -1;
            retval = true;
        }
        return retval;
    }

    bool redraw()
    {
        bool retval = false;
        if (drawStatus == 0)
        {
            drawFrame();
            drawStatus = 1;
        }
        else if (drawStatus == 1)
        {
            if (show(h_tempC, 10, grad_min_T, grad_max_T, scale_T, color_tempe, 416))
            {
                drawStatus = 2;
            }
        }
        else if(drawStatus == 2)
        {
            bool ds;
            switch (displayMode)
            {
            case eDisplayMode::eTempHum:
                ds = show(h_hum, 1, grad_min_H, grad_max_H, scale_H, color_humid, 448);
                break;
            case eDisplayMode::eTempPress:
                ds = show(h_pressHmm, 1, grad_min_P, grad_max_P, scale_P, color_press, 448);
                break;
            case eDisplayMode::eTempCO2:
                ds = show(h_co2ppm, 1, grad_min_C, grad_max_C, scale_C, color_co2, 448); 
                break;
            case eDisplayMode::eTempTVOC:
                ds = show(h_tvoc, 1, grad_min_Tv, grad_max_Tv, scale_Tv, color_tvoc, 448);
                break;
            }
            if (ds)
            {
                retval = true;
                drawStatus = 0;
            }
        }
        return retval;
    }
};

#undef DIAGRAMS__CPP