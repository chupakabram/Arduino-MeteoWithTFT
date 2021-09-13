#define TIMER__CPP

#include <arduino.h>

#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library

#include <Wire.h>
#include "RTClib.h"

#include "Common.h"
#include "Sticker.h"

#include "Timer.h"

#define DEBUG false 
#define DEBUGPRINT(x)   if(DEBUG){Serial.println(x);}

//#undef DS3232_EXISTS

//#define USE_STICKER

#define RESET_CLOCK    false

namespace MeteoMega::Timer
{
    typedef enum 
    {
        eNormalMode = 0,
        eSetupMode
    } eMode;

    typedef enum
    {
        eYear = 0,
        eMonth,
        eDay,
        eHour, 
        eMinute
    } eSetupStage;

    uint32_t clockTimer;
    MeteoMega::Sticker::Sticker * initSticker;
    MeteoMega::Sticker::Sticker * sticker;
    int pollingInterval_ms;

    char* weekdayNames[]={(char*)"Sun ", (char*)"Mon ", (char*)"Tue ", (char*)"Wed ", (char*)"Thu ", (char*)"Fri ", (char*)"Sat "};
    uint8_t daysPerMonth [2][12] = {{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}, {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};

    RTC_DS3231 rtc;

    Time tm;

    Time tmForSetting;

    eMode mode = eNormalMode;
    eSetupStage setupStage = eYear;

    DateTime now;

    String dayMonth;
    String hms;
    
    uint8_t prevSec;
    uint8_t prevMin;
    uint8_t prevDow;

    uint8_t dayMonthDOW[10];
    uint8_t hhmmss[9];

    uint8_t yy[3],mm[3],dd[3];
    uint8_t hh[3], mn[3];
    const char * slash = "/";
    const char * dots = ":";

    LCDWIKI_KBV * _lcd;

    void (*pDrawPointer)();

#ifndef DS3232_EXISTS
    uint16_t tickCounter;
#endif
    bool sensorExists = false;

    Time readTime()
    {
        return tm;
    }

    void InitTimer( MeteoMega::Sticker::Sticker * pInitSticker, 
                    MeteoMega::Sticker::Sticker * pSticker, 
                    int pollingInterval)
    {
        clockTimer = millis();
        pollingInterval_ms = pollingInterval;
        initSticker = pInitSticker;
        sticker = pSticker; 

        tm.hour = 0;
        tm.min = 56;
        tm.sec = 48;
        tm.month = 12;
        tm.day = 21;
        tm.dayOfWeek = 0;

        prevSec = 61;
        prevMin = 61;
        prevDow = 8;

        pDrawPointer = drawFirstTime;

        initDS3231();
    }

    void UpdateTimer()
    {
        if( ( millis()-clockTimer ) > ((unsigned long)pollingInterval_ms))
        {
            clockTimer = millis();
            acquireTime();
#ifdef USE_STICKER
            drawWithSticker();
#else            
            pDrawPointer();
#endif
        }
    }

    void initDS3231()
    {
        initSticker->Show_text((char *)"Init ...");

#ifdef DS3232_EXISTS
        if (!rtc.begin())
        {
            DEBUGPRINT(F("Couldn't find RTC"));
            initSticker->Show_text((char *)"FAILED");
        }
        else
        {
            if (RESET_CLOCK || rtc.lostPower())
            {
                rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
            }

            DEBUGPRINT(F("RTC is found."));
            initSticker->Show_text((char *)"OK");
            sensorExists = true;
        }
#else
        tickCounter = 0;
        initSticker->Show_text((char *)"OK");
        sensorExists = true;
#endif
    }

    void acquireTime()
    {
#ifdef DS3232_EXISTS
        now = rtc.now();
        tm.sec = now.second();
        tm.hour = now.hour();
        tm.min = now.minute();
        tm.month = now.month();
        tm.day = now.day();
        tm.year = now.year();
        tm.dayOfWeek = now.dayOfTheWeek();
#else
        tickCounter = (tickCounter + 1 ) % 30000;
        tm.hour = (tm.hour + (0 == tickCounter % 40 ? 1 : 0)) % 24; 
        tm.sec = (tm.sec + (0 == tickCounter % 2 ? 1 : 0)) % 60; 
        tm.dayOfWeek =  (tm.dayOfWeek + (0 == tickCounter % 240 ? 1 : 0)) % 7; 
#endif
    }

    void drawWithSticker()
    {
        if (prevDow != tm.dayOfWeek)
        {
            prevDow = tm.dayOfWeek;
            dayMonth = " " + String(weekdayNames[tm.dayOfWeek]) + String(tm.day) + "." + String(tm.month);
            sticker->SetTitle((char *)dayMonth.c_str());
        }

        if (prevSec != tm.sec)
        {
            prevSec = tm.sec;
            hms = String(tm.hour) + ":" + String(tm.min) + ":" + String(tm.sec) + " ";
            sticker->Show_string(hms);
        }
    }

    void drawFirstTime()
    {
        sticker->lcd->Set_Draw_color(sticker->colour_back);
        sticker->lcd->Fill_Rectangle(sticker->x0 + 1, sticker->y0 + 1, sticker->x0 + sticker->dx - 1,sticker-> y0 + sticker->dy - 1);
        sticker->lcd->Set_Draw_color(sticker->colour_frame);
        sticker->lcd->Draw_Rectangle(sticker->x0, sticker->y0, sticker->x0 + sticker->dx, sticker->y0 + sticker->dy);
        if(mode == eNormalMode)
        {
            pDrawPointer = draw;
        }
        else
        {
            pDrawPointer = drawSetupMode;
        }
    }

    void draw()
    {
        if (prevDow != tm.dayOfWeek)
        {
            prevDow = tm.dayOfWeek;
            for(int ii=0;ii<3;++ii) dayMonthDOW[ii] = weekdayNames[tm.dayOfWeek][ii];
            dayMonthDOW[3] =' ';
            dayMonthDOW[4] = tm.day / 10 + 48;
            dayMonthDOW[5] = tm.day % 10 + 48;
            dayMonthDOW[6] = '.';
            dayMonthDOW[7] = tm.month / 10 + 48;
            dayMonthDOW[8] = tm.month % 10 + 48;
            dayMonthDOW[9] = '\0';

            sticker->lcd->Set_Text_Back_colour(sticker->colour_back);
            sticker->lcd->Set_Text_colour(sticker->colour_name);
            sticker->lcd->Set_Text_Size(2);
            sticker->lcd->Print_String((char *)dayMonthDOW, sticker->x0+6, sticker->y0+6);
        }

        //if (prevMin != tm.min)
        //{
            prevMin = tm.min;
            hhmmss[0] = tm.hour / 10 + 48;
            hhmmss[1] = tm.hour % 10 + 48;
            hhmmss[2] = ':';
            hhmmss[3] = tm.min / 10 + 48;
            hhmmss[4] = tm.min % 10 + 48;
            hhmmss[5] = ':';
            hhmmss[6] = '\0';
        //}

        if (prevSec != tm.sec)
        {
            prevSec = tm.sec;
            hhmmss[6] = tm.sec / 10 + 48;
            hhmmss[7] = tm.sec % 10 + 48;
            hhmmss[8] = '\0';

            sticker->lcd->Set_Text_Back_colour(sticker->colour_back);
            sticker->lcd->Set_Text_colour(sticker->colour_txt);
            sticker->lcd->Set_Text_Size(3);
            sticker->lcd->Print_String((char *)hhmmss, sticker->x0+15, sticker->y0+28);
        }
    }

    // functions to support the time settings mode
    void startSetup()
    {
        tmForSetting = tm;
        tmForSetting.year = tmForSetting.year % 100; 
        mode = eSetupMode;
        setupStage = eYear;
        pDrawPointer = drawFirstTime;
    }
    void cancelSetup()
    {
        mode = eNormalMode;
        prevDow = 8; // to make the date row to update  
        pDrawPointer = drawFirstTime;
    }
    void increaseParameter()
    {
        switch(setupStage) 
        {
            case eDay:   
                        {
                            tmForSetting.day = tmForSetting.day + 1;
                            if ( ((uint8_t) tmForSetting.day) > daysPerMonth[1][tmForSetting.month-1] && tmForSetting.year % 4 == 0)
                            {
                                tmForSetting.day = 1;
                            } 
                            if ( ((uint8_t)tmForSetting.day) > daysPerMonth[0][tmForSetting.month-1] && tmForSetting.year % 4 != 0)
                            {
                                tmForSetting.day = 1;
                            } 
                            break;
                        }
            case eMonth: 
                        {
                            tmForSetting.month = (tmForSetting.month + 1) % 13; 
                            tmForSetting.month = tmForSetting.month == 0 ? 1 : tmForSetting.month; 
                            break;
                        }
            case eYear: 
                        {
                            tmForSetting.year = (tmForSetting.year + 1 ) % 100; 
                            break;
                        }
            case eHour:
                        {
                            tmForSetting.hour = (tmForSetting.hour + 1 ) % 24; 
                            break;
                        }
            case eMinute:
                        {
                            tmForSetting.min = (tmForSetting.min + 1 ) % 60;
                            break;
                        }   
        }
    }
    bool saveParameterGoToNext()
    {
        bool retVal = false;
        switch(setupStage) 
        {
            case eYear: 
                        setupStage = eMonth; break;
            case eMonth: 
                        setupStage = eDay; break;
            case eDay:   
                        setupStage = eHour; break;
           case eHour:
                        setupStage = eMinute; break;
            case eMinute:
                        mode = eNormalMode;
                        DateTime timeToSet( 2000 + tmForSetting.year, tmForSetting.month, tmForSetting.day, tmForSetting.hour, tmForSetting.min, 0);
#ifdef DS3232_EXISTS                        
                        rtc.adjust(timeToSet);
#else
                        tm = tmForSetting; tm.year += 2000; 
#endif
                        prevDow = 8; // to make the date row to update  
                        pDrawPointer = drawFirstTime;
                        retVal = true;
                        break;
        }
        return retVal;
    }
  
    void drawSetupMode()
    {
        // TODO - add drawing

        //  yy / mm / dd            starts from +6, +6     size 2
        //    hh / mm               starts from +15,+28    size 3 

        prepareStringToDraw(yy,tmForSetting.year);
        prepareStringToDraw(mm,tmForSetting.month);
        prepareStringToDraw(dd,tmForSetting.day);
        prepareStringToDraw(hh,tmForSetting.hour);
        prepareStringToDraw(mn,tmForSetting.min);

        drawParameter(yy, sticker->x0+12, sticker->y0+6, 2, setupStage == eYear ? RED : sticker->colour_back);
        drawParameter((uint8_t*)slash, sticker->x0+46, sticker->y0+6, 2, sticker->colour_back);
        drawParameter(mm, sticker->x0+62, sticker->y0+6, 2, setupStage == eMonth ? RED : sticker->colour_back);
        drawParameter((uint8_t*)slash, sticker->x0+96, sticker->y0+6, 2, sticker->colour_back);
        drawParameter(dd, sticker->x0+112, sticker->y0+6, 2, setupStage == eDay ? RED : sticker->colour_back);

        drawParameter(hh, sticker->x0+15, sticker->y0+28, 3, setupStage == eHour ? RED : sticker->colour_back);
        drawParameter((uint8_t*)dots, sticker->x0+55, sticker->y0+28, 3, sticker->colour_back);
        drawParameter(mn, sticker->x0+75, sticker->y0+28, 3, setupStage == eMinute ? RED : sticker->colour_back);
    }

    void prepareStringToDraw(uint8_t *arr, int val)
    {
        arr[0] = val / 10 + 48;
        arr[1] = val % 10 + 48;
        arr[2] = '\0';
    }

    void drawParameter(uint8_t * arr, uint16_t x, uint16_t y, uint16_t size, uint16_t backColor)
    {
        sticker->lcd->Set_Text_Back_colour(backColor);
        sticker->lcd->Set_Text_colour(sticker->colour_name);
        sticker->lcd->Set_Text_Size(size);
        sticker->lcd->Print_String((char *)arr, x, y);
    }
};

#undef TIMER__CPP