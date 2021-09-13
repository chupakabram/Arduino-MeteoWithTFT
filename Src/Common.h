#ifndef COMMON__H
#define COMMON__H 

#include <stdint.h>
#include <Arduino.h>

// Global common definitions

// define some color values
#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define ORANGE 		    0xFBC0
#define YELLOW_PALE     0xF7F4
#define BLUE_LIGHT      0x455F
#define VIOLET		    0x781A
#define GRAY_VERY_LIGHT 0xDEFB
#define GRAY_LIGHT      0xA534
#define GRAY   		    0x8410
#define GRAY_DARK       0x5ACB

#define MAIN_BACKGROUND_COLOR  0x18F7  // 0x2020c4 : 32,32,196

// --------------------------------------------------

#define HIGH_CO2_THRESHOLD      1200
#define MIDDLE_CO2_THRESHOLD    800

#define NORMAL_CO2              400
#define NORMAL_TVOC             50

// --------------------------------------------------

#define PHOTORESISTOR_PIN   A15

#define TX_PIN  18
#define RX_PIN  19

#define SDA_PIN     20
#define SCL_PIN     21

#define RESET_BUTTON_PIN 24
#define MODE_BUTTON_PIN  23

#define LED_R_PIN   44
#define LED_G_PIN   45
#define LED_B_PIN   46


//// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//#define EMULATION_MODE

#ifdef EMULATION_MODE
    #define CLOCK_TIMEOUT       500
    #define BME280_TIMEOUT      29000
    #define CCS811_IMEOUT       31000
    #define ESP8266_TIMEOUT     900000  

    #ifdef BME280_EXISTS
        #undef BME280_EXISTS
    #endif
    #ifdef CCS811_EXISTS
        #undef CCS811_EXISTS
    #endif
    #ifdef DS3232_EXISTS
        #undef DS3232_EXISTS
    #endif
    #ifdef ESP8266_EXISTS
        #undef ESP8266_EXISTS
    #endif
#else
    #define CLOCK_TIMEOUT       500
    #define BME280_TIMEOUT      29000
    #define CCS811_IMEOUT       31000
    #define ESP8266_TIMEOUT     900000  

    #define BME280_EXISTS
    #define CCS811_EXISTS
    #define DS3232_EXISTS
    #define ESP8266_EXISTS
#endif

#endif   /// COMMON__H