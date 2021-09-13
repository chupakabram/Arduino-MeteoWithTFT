#ifndef STATEMACHINE__H
#define STATEMACHINE__H 

#include <pins_arduino.h>

#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library

#include <OneButton.h>

#include "Common.h"
#include "Sticker.h"

namespace MeteoMega::StateMachine
{
    class StateMachine
    {
        public:
            StateMachine();
            virtual ~StateMachine();

            void Init();
            void Do();
        private:
            void initStickersInitialization();
            void initStickersSensors();
            void clear_screen();
        private:
            static LCDWIKI_KBV _Lcd;
        private: 
            LCDWIKI_KBV * pMyLcd;

            OneButton * resetCO2Button;
            OneButton * setupButton;

            Sticker::Sticker stickerTimer;
            Sticker::Sticker stickerTemperature;
            Sticker::Sticker stickerPressure;
            Sticker::Sticker stickerHumidity;
            Sticker::Sticker stickerCO2;
            Sticker::Sticker stickerTVOC; 
			
            Sticker::Sticker stickerBME280Init;
            Sticker::Sticker stickerCCS811Init;
            Sticker::Sticker stickerDS3231Init;
            Sticker::Sticker stickerESP8266Init;
    };
};

#endif /// STATEMACHINE__H