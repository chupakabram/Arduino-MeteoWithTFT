#define ESP8266___CPP

#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library

#include <SoftwareSerial.h>
#include <Wire.h>

#include "Common.h"
#include "Sensors.h"

#include "ESP8266.h"

#define NETWORK  "*************"                                // your access point SSID
#define PASSWORD "**********"                                   // your wifi Access Point password

#define IP "184.106.153.149"                                    // IP address of thingspeak.com
#define GETSTRING "GET /update?api_key=****************"        // replace with your channel key

// https://thingspeak.com/channels/1481727/private_show
// Channel -  name 'Train_5_parameters_000' , id '1481727'
// GET https://api.thingspeak.com/update?api_key=1CYSV23WHUY65XDO&field1=0

#define DEBUG false 
#define DEBUGPRINT(x)   if(DEBUG){Serial.println(x);}

//#undef ESP8266_EXISTS

namespace MeteoMega::ESP8266
{
    typedef enum
    {
        Idle = 0,
        Setup,
        ChangeMode,
        ConnectWifi,
        ConnectTCP,
        SendData,
      } eStage;

    int stageTimer;
    eStage stage;
    bool stageDone = false;

    SoftwareSerial esp8266Module(RX_PIN, TX_PIN);  

    uint32_t clockTimer;
    MeteoMega::Sticker::Sticker * initSticker;
    uint32_t sendingInterval_ms;

    bool deviceOK = false;

    LCDWIKI_KBV *lcd;

    void InitESP8266(MeteoMega::Sticker::Sticker *pStickerInit, LCDWIKI_KBV *lcd_i, uint32_t sendingInterval)
    {
        initSticker = pStickerInit;
#ifdef ESP8266_EXISTS
        sendingInterval_ms = sendingInterval;
#else
        sendingInterval_ms = 90000;
#endif

        lcd = lcd_i;

        initSticker->Show_text((char *)"Init ...");

#ifdef ESP8266_EXISTS
        esp8266Module.begin(9600);                        // Setting softserial baud rate to 9600

        delay(5000);
        DEBUGPRINT(F("Reseting esp8266"))

        esp8266Module.flush();
        esp8266Module.println(F("AT+RST"));
        delay(7000);
        if (esp8266Module.find((char*)"OK"))
        {
            DEBUGPRINT(F("Reset esp8266 - OK"))
            initSticker->Show_text((char *)"OK");
            deviceOK = true;
        }
        else
        {
            DEBUGPRINT(F("Failed to reset esp8266"))
            initSticker->Show_text((char *)"FAILED");
        }
#else
        initSticker->Show_text((char *)"OK");
        deviceOK = true;
#endif
        stage = Idle;
        showStatus();
        clockTimer = millis(); 
    }
    
    void UpdateESP8266()
    {
        switch (stage)
        {
            case Idle:
                if( ( millis()-clockTimer ) > ((unsigned long)sendingInterval_ms))
                {
                    clockTimer = millis();
                    showStatus();
#ifdef ESP8266_EXISTS
                    stage = Setup;
                    stageDone = false;
                    deviceOK = false;    // should be set 'true' when finishing to send data
#else
                    DEBUGPRINT(F("Update esp8266"));
#endif
                } 
                break;
            case Setup:
                setupESP8266();
                break;
            case ChangeMode:
                changingMode();
                break;
            case ConnectWifi:
                connectToWiFi();
                break;
            case ConnectTCP:
                connectTCP();
                break;
            case SendData:
                sendData();
                break;
        }
    }

    void showStatus()
    {
        lcd->Set_Text_Back_colour(MAIN_BACKGROUND_COLOR);
        lcd->Set_Text_Size(1);
        if (deviceOK)
        {
            lcd->Set_Text_colour(GREEN);
            lcd->Print_String((char *)"WiFi is ON  ", 400, 6);
        }
        else
        {
            lcd->Set_Text_colour(RED);
            lcd->Print_String((char *)"WiFi is OFF ", 400, 6);
        }
    }

    void setupESP8266()
    {
        if (!stageDone)
        {
            DEBUGPRINT(F("Resetting esp8266"))
            esp8266Module.flush();
            esp8266Module.println(F("AT+RST"));
            stageDone = true;
            stageTimer = millis();
        }
        else
        {
            if ((millis() - stageTimer) > ((unsigned long)7000))
            {
                if (esp8266Module.find((char*)"OK"))
                {
                    DEBUGPRINT(F("Found OK"))
                    stage = ChangeMode;
                }
                else
                {
                    DEBUGPRINT(F("OK not found"))
                    stage = Idle;
                }
                stageDone = false;
            }
        }
    }

    void changingMode()
    {
        if (!stageDone)
        {
            esp8266Module.flush();
            esp8266Module.println(F("AT+CWMODE=1"));
            stageDone = true;
            stageTimer = millis();
        }
        else
        {
            if ((millis() - stageTimer) > ((unsigned long)5000))
            {
                if (esp8266Module.find((char*)"OK"))
                {
                    DEBUGPRINT(F("Mode changed"))
                    stage = ConnectWifi;
                }
                else if (esp8266Module.find((char *)"NO CHANGE"))
                {
                    DEBUGPRINT(F("Already in mode 1"))
                    stage = ConnectWifi;
                }
                else
                {
                    DEBUGPRINT(F("Error while changing mode"))
                    stage = Idle;
                }
                stageDone = false;
            }
        }
    }

    void connectToWiFi()
    {
        if (!stageDone)
        {
            DEBUGPRINT(F("Inside connectToWiFi"))
            String cmd = F("AT+CWJAP=\"");
            cmd += NETWORK;
            cmd += F("\",\"");
            cmd += PASSWORD;
            cmd += F("\"");
            esp8266Module.println(cmd);
            stageDone = true;
            stageTimer = millis();
        }
        else
        {
            if ((millis() - stageTimer) > ((unsigned long)15000))
            {
                if (esp8266Module.find((char *)"OK"))
                {
                    DEBUGPRINT(F("Connected to Access Point"))
                    stage = SendData;
                }
                else
                {
                    DEBUGPRINT(F("Could not connect to Access Point"))
                    stage = Idle;
                }
                stageDone = false;
            }
        }
    }

    void connectTCP()
    {
        if (!stageDone)
        {
            DEBUGPRINT(F("Inside connectTCP"))
            String cmd = "AT+CIPSTART=\"TCP\",\"";
            cmd += IP;
            cmd += "\",80";
            esp8266Module.println(cmd);
            stageDone = true;
            stageTimer = millis();
        }
        else
        {
            if ((millis() - stageTimer) > ((unsigned long)5000))
            {
                if (esp8266Module.find((char *)"Error"))
                {
                    DEBUGPRINT(F("ERROR while connecting TCP"))
                    stage = Idle;
                }
                else
                {
                    DEBUGPRINT(F("TCP connection OK"))
                    stage = SendData;
                }
                stageDone = false;
            }
        }
    }

    void sendData()
    {
        String cmd;
        if (!stageDone)
        {
            DEBUGPRINT(F("Inside sendData"))
            Sensors::Sensors sn = Sensors::pollSensors();
            cmd = GETSTRING +   String("&field1=") + String(sn.tempC) + 
                                String("&field2=") + String(sn.pressHmm) + 
                                String("&field3=") + String(sn.hum) + 
                                String("&field4=") + String(sn.co2ppm) +  
                                String("&field5=") + String(sn.tvoc) + String("\r\n");
            esp8266Module.print("AT+CIPSEND=");
            esp8266Module.println(cmd.length());
            stageDone = true;
            stageTimer = millis();
        }
        else
        {
            if ((millis() - stageTimer) > ((unsigned long)15000))
            {
                if (esp8266Module.find((char *)">"))
                {
                    esp8266Module.print(cmd);
                    DEBUGPRINT(F("Data sent"))
                    deviceOK = true;
                }
                else
                {
                    esp8266Module.println("AT+CIPCLOSE");
                    DEBUGPRINT(F("Connection closed"))
                }
                stage = Idle;
                stageDone = false;
            }
        }
    }
};

#undef ESP8266___CPP
