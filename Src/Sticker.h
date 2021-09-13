
#ifndef STICKER__H
#define STICKER__H 

// based on http://silicium628.fr/articles/148/documents/Firmware_C/src/Station_meteo.h
// http://silicium628.fr/article_i.php?id=148
/*************************************************************************************
    CLASS Sticker  // shows a number or a short text in rectangle						
***************************************************************************************/

#include <stdint.h>
#include <Arduino.h>

class LCDWIKI_KBV;

namespace MeteoMega::Sticker
{
    class Sticker
    {
    public:
        Sticker() : lcd(nullptr){};
        virtual ~Sticker(){};

        void Init(LCDWIKI_KBV *lcd_i, uint16_t x, uint16_t y, uint16_t dx, uint16_t dy, char name[20]);
	    void SetColourFrame(uint16_t couleur_i);
	    void SetColourName(uint16_t couleur_i);
	    void SetColourTxt(uint16_t couleur_i);
	    void SetColourBack(uint16_t couleur_i);
        void SetUnit(char txt_unit_i[3]);
        void SetTitle(char name[20]);
        void SetMinMax(uint16_t v_min, uint16_t v_max);
        void SetMinMax(float v_min, float v_max);
        void Clear();
        void FlashBackground(uint16_t color_i);
        void Show_int(uint32_t value, uint8_t nb_num);
        //void Show_int(uint32_t value, uint8_t nb_num, char txt_unit_i[3], uint32_t v_min, uint32_t v_max);
        void Show_float(float value, uint8_t nb_num, uint8_t nb_dec);
        //void Show_float(float value, uint8_t nb_num, uint8_t nb_dec, char txt_unit_i[3], float v_min, float v_max);
        void Show_HEX(uint32_t value);
        void Show_text(char txt_i[10]);
        void Show_string(String txt_i);
 
    public:
        uint16_t x0; //position
        uint16_t y0;
        uint16_t dx; //dimension
        uint16_t dy;

        uint16_t vmin_int;
        uint16_t vmax_int;
        float    vmin_float;
        float    vmax_float;

        // colors
	    uint16_t colour_frame;
	    uint16_t colour_back;
	    uint16_t colour_name;
	    uint16_t colour_txt;

        LCDWIKI_KBV *lcd;

    private:
        //void traceBorder();
        //void showName();
        //void effect();

    private:
        void drawTitle();
        void drawMinMaxInt();
        void drawMinMaxFloat();
        void drawUnit();
        void clearInsideFrame();
        void clearMainArea();
        void clearMaxMinArea();

    private:
        
        char name[21];
        char txt_unit[4];
        char text[11];

        bool m_renderTitle;
        bool m_renderUnit;
        bool m_renderMinMax;
    };
};

#endif   /// STICKER__H
