#define STATEMACHINE__CPP

#include "Common.h"

#include "Bme280.h"
#include "Timer.h"
#include "CCS811.h"
#include "ESP8266.h"
#include "Sensors.h"
#include "SetupController.h"
#include "PhotoResistor.h"
#include "LED.h"
#include "Diagrams.h"

#include "StateMachine.h"

namespace MeteoMega::StateMachine
{
    //if the IC model is known or the modules is unreadable,you can use this constructed function
    LCDWIKI_KBV StateMachine::StateMachine::_Lcd (ILI9486,A3,A2,A1,A0,A4); 
    //if the IC model is not known and the modules is readable,you can use this constructed function
    //LCDWIKI_KBV StateMachine::StateMachine::_Lcd (320,480,A3,A2,A1,A0,A4);//width,height,cs,cd,wr,rd,reset

    StateMachine::StateMachine()
    {   
        pMyLcd = &_Lcd; 
    }
    StateMachine::~StateMachine()
    {
    }
    void StateMachine::Init()
    {
        // Initialize TFT
        Serial.begin(9600);
        pMyLcd->Init_LCD();
        Serial.print("TFT display ID: ");
        Serial.println(pMyLcd->Read_ID(), HEX);

        clear_screen();
	    delay(100);
	    pMyLcd->Set_Rotation(1);

        // Initialize the 'initialization stickers'
        initStickersInitialization();

        // Init the RCS - timer
        MeteoMega::Timer::InitTimer(&stickerDS3231Init, &stickerTimer, CLOCK_TIMEOUT);

        // Init BM280   - temperature, pressure, humidty 
        MeteoMega::BME280::InitBME280(&stickerBME280Init, &stickerHumidity, &stickerTemperature, &stickerPressure, BME280_TIMEOUT);

        // Init CCS811  - CO2, TVOC (Total volatile organic compounds)
        MeteoMega::CCS811::InitCCS811(&stickerCCS811Init, &stickerCO2, &stickerTVOC, CCS811_IMEOUT);

        // Init ESP8266 - WiFi
        MeteoMega::ESP8266::InitESP8266(&stickerESP8266Init, pMyLcd, ESP8266_TIMEOUT);

        MeteoMega::SetupController::Init();

        delay(4000);
        clear_screen();

        pMyLcd->Set_Text_Back_colour(32, 32, 192);
        pMyLcd->Set_Text_colour(YELLOW);
        pMyLcd->Set_Text_Size(1);
        pMyLcd->Print_String((char *)"Meteo ver.1.0", 6, 6);
	  
        // Initialize the 'stickers'
        initStickersSensors();

        // Initialize the 'Diagrams' object
        MeteoMega::Diagrams::SetBackgroundColor(BLUE);
        MeteoMega::Diagrams::SetFrameColor(YELLOW);

        MeteoMega::Diagrams::SetTemperatureColor(GREEN);
        MeteoMega::Diagrams::SetHumidityColor(WHITE);
        MeteoMega::Diagrams::SetPressureColor(YELLOW);
        MeteoMega::Diagrams::SetCO2Color(WHITE);
        MeteoMega::Diagrams::SetTVOCColor(YELLOW);

        MeteoMega::PhotoResistor::InitPhotoresistor();
        MeteoMega::LED::InitLED();

        resetCO2Button = new OneButton(
            RESET_BUTTON_PIN, // Input pin for the button
            false             // Button is active HIGH
        );
        setupButton = new OneButton(
            MODE_BUTTON_PIN, // Input pin for the button
            false             // Button is active HIGH
        );

        resetCO2Button->setDebounceTicks(50);
        resetCO2Button->setPressTicks(7000);

        setupButton->setDebounceTicks(50);
        setupButton->setClickTicks(200);
        setupButton->setPressTicks(1000);

        resetCO2Button->attachLongPressStop([]
                                         { MeteoMega::CCS811::CalibrateCO2(); });

        setupButton->attachClick([]
                                 { MeteoMega::SetupController::ClickHandler(); });
        setupButton->attachLongPressStop([]
                                 { MeteoMega::SetupController::PressHandler(); });
        setupButton->attachDoubleClick([]
                                       { MeteoMega::SetupController::DoubleClickHandler(); });

        MeteoMega::Diagrams::InitDiagrams(pMyLcd, 0, 150, 480, 170);
    }

    void StateMachine::Do()
    {
        resetCO2Button->tick();
        setupButton->tick();

        MeteoMega::Timer::UpdateTimer();
        MeteoMega::BME280::UpdateBME280();
        MeteoMega::CCS811::UpdateCCS811();
        MeteoMega::ESP8266::UpdateESP8266();

        MeteoMega::SetupController::Update();

        MeteoMega::PhotoResistor::UpdatePhotoresistor();
        MeteoMega::LED::SetBright(!MeteoMega::PhotoResistor::IsDark());

        Sensors::Sensors sn = Sensors::pollSensors();
        if (sn.co2ppm > HIGH_CO2_THRESHOLD)
        {
            MeteoMega::LED::SetColor(MeteoMega::LED::eColor::eHigh);
        }
        else if (sn.co2ppm > MIDDLE_CO2_THRESHOLD)
        {
            MeteoMega::LED::SetColor(MeteoMega::LED::eColor::eMiddle);
        }
        else
        {
            MeteoMega::LED::SetColor(MeteoMega::LED::eColor::eNormal);
        }

        MeteoMega::LED::UpdateLED();

        MeteoMega::Diagrams::UpdateDiagrams(); 
    }

    void StateMachine::clear_screen()
    {
        pMyLcd->Fill_Screen(MAIN_BACKGROUND_COLOR);
    }
	
	void StateMachine::initStickersInitialization()
    {
		// Initialize the 'initialization stickers
	    int h = 16;
		int dH = 76;
        stickerDS3231Init.SetColourBack(BLUE);
    	stickerDS3231Init.SetColourTxt(ORANGE);
	    stickerDS3231Init.SetColourFrame(YELLOW);
	    stickerDS3231Init.SetColourName(WHITE);
        stickerDS3231Init.Init(pMyLcd, 150, h, 180,60,  (char*)"DS3231");
       		
		h += dH;
		stickerBME280Init.SetColourBack(GREEN);      
		stickerBME280Init.SetColourTxt(RED);       
		stickerBME280Init.SetColourFrame(BLUE);		
		stickerBME280Init.SetColourName(BLACK);
		stickerBME280Init.Init(pMyLcd, 150, h, 180,60,  (char*)"BME280");

		h += dH;
        stickerCCS811Init.SetColourBack(VIOLET);
    	stickerCCS811Init.SetColourTxt(GRAY_LIGHT);
	    stickerCCS811Init.SetColourFrame(GREEN);
	    stickerCCS811Init.SetColourName(BLACK);
		stickerCCS811Init.Init(pMyLcd, 150, h, 180,60,  (char*)"CCS811");
       		
		h += dH;
	    stickerESP8266Init.SetColourBack(YELLOW_PALE);
    	stickerESP8266Init.SetColourTxt(GREEN);
	    stickerESP8266Init.SetColourFrame(BLUE);
	    stickerESP8266Init.SetColourName(MAGENTA);
		stickerESP8266Init.Init(pMyLcd, 150, h, 180,60,  (char*)"ESP8266");
	}

	void StateMachine::initStickersSensors()
    {
        // Initialize the 'stickers'
        int h = 18;
		int dH = 68;
		
         // Initialize the 'humidity' sticker
        stickerHumidity.SetColourBack(GRAY_VERY_LIGHT);
    	stickerHumidity.SetColourTxt(BLUE);
	    stickerHumidity.SetColourFrame(CYAN);
	    stickerHumidity.SetColourName(RED);
        stickerHumidity.Init(pMyLcd, 2, h, 148, 60, (char*)"Humidity");

         // Initialize the 'pressure' sticker
        stickerPressure.SetColourBack(MAGENTA);
    	stickerPressure.SetColourTxt(YELLOW);
	    stickerPressure.SetColourFrame(ORANGE);
	    stickerPressure.SetColourName(WHITE);
        stickerPressure.Init(pMyLcd, 152, h, 148, 60, (char*)"Pressure");

         // Initialize the 'temperature' sticker
        stickerTemperature.SetColourBack(GREEN);
    	stickerTemperature.SetColourTxt(RED);
	    stickerTemperature.SetColourFrame(BLUE);
	    stickerTemperature.SetColourName(BLACK);
        stickerTemperature.Init(pMyLcd, 302, h, 176,60, (char*)"Temperature");

	    h += dH;
         // Initialize the 'CO2' sticker
        stickerCO2.SetColourBack(VIOLET);
    	stickerCO2.SetColourTxt(BLACK);
	    stickerCO2.SetColourFrame(GREEN);
	    stickerCO2.SetColourName(GREEN);
        stickerCO2.Init(pMyLcd, 2, h, 148, 60, (char*)"CO2");

         // Initialize the 'TVOC' sticker
        stickerTVOC.SetColourBack(YELLOW_PALE);
    	stickerTVOC.SetColourTxt(0x03E0 /*GREEN*/);
	    stickerTVOC.SetColourFrame(BLUE);
	    stickerTVOC.SetColourName(MAGENTA);
        stickerTVOC.Init(pMyLcd, 152, h, 148, 60, (char*)"TVOC");
        
		// Initialize the 'timer' sticker
        stickerTimer.SetColourBack(BLUE);
    	stickerTimer.SetColourTxt(ORANGE);
	    stickerTimer.SetColourFrame(YELLOW);
	    stickerTimer.SetColourName(WHITE);
        stickerTimer.Init(pMyLcd, 302, h, 176, 60, (char*)"Time");
	}
};

#undef STATEMACHINE__CPP