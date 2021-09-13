#define STICKER__CPP

#include "Common.h"
#include "Sticker.h"

#include <LCDWIKI_GUI.h> //Core graphics library
#include <LCDWIKI_KBV.h> //Hardware-specific library

namespace MeteoMega::Sticker
{
    void Sticker::Init(LCDWIKI_KBV *lcd_i, uint16_t xi, uint16_t yi, uint16_t dxi, uint16_t dyi, char name_i[18])
    {
        x0 = xi;
        y0 = yi;
        dx = dxi;
        dy = dyi;

        lcd = lcd_i;

        SetTitle(name_i);

        //lcd->Set_Text_Back_colour(colour_back);
        //lcd->Set_Text_colour(colour_name);

        m_renderTitle = true;

        m_renderMinMax = false;
        m_renderUnit = true;

        //traceBorder();
        //showName();

        drawTitle();
    }
    void Sticker::SetColourFrame(uint16_t color_i)
    {
        colour_frame = color_i;
    };
    void Sticker::SetColourName(uint16_t color_i)
    {
        colour_name = color_i;
    };
    void Sticker::SetColourTxt(uint16_t color_i)
    {
        colour_txt = color_i;
    };
    void Sticker::SetColourBack(uint16_t color_i)
    {
        colour_back = color_i;
    };
    void Sticker::SetUnit(char txt_unite_i[3])
    {
        for (int i = 0; i < 3; i++) { txt_unit[i] = txt_unite_i[i]; }
        txt_unit[3] = '\0'; // zero terminal
        m_renderUnit = true;    
    }
    void Sticker::SetTitle(char name_i[20])
    {
        for (int ii = 0; ii < 20; ii++) { name[ii] = name_i[ii]; }
        name[20] = '\0'; // zero terminal  
        m_renderTitle = true;    
    }
    void Sticker::SetMinMax(uint16_t v_min, uint16_t v_max)
    {
        vmin_int = v_min;
        vmax_int = v_max;
        m_renderMinMax = true;
    }
    void Sticker::SetMinMax(float v_min, float v_max)
    {
        vmin_float = v_min;
        vmax_float = v_max;
        m_renderMinMax = true;
    }
    void Sticker::Clear(){};
    void Sticker::FlashBackground(uint16_t color_i){};
    void Sticker::Show_int(uint32_t value, uint8_t nb_chiffres)
    {
        drawTitle();
        drawMinMaxInt();
        drawUnit();
        clearMainArea();

        uint16_t z = 0;
        lcd->Set_Text_Back_colour(colour_back);
        lcd->Set_Text_colour(colour_txt);
        lcd->Set_Text_Size(4);
        if ((value < 1000) && (value > 100)) { z = 20; }
        lcd->Print_Number_Int(value, x0 + 10 + z, y0 + 21, nb_chiffres, ' ', 10);
    }
    // void Sticker::Show_int(uint32_t value, uint8_t nb_chiffres, char txt_unite_i[3], uint32_t v_min, uint32_t v_max)
    // {
    //     // 'valeur' sera afficée en gros, 'v_min' et 'v_max' en plus petit dans le coin du cadre (facultativement)
    //     // pour ne pas afficher 'v_min' et 'v_max', il faut que toutes les deux soient ==0

    //     uint16_t z;
    //     SetUnit(txt_unite_i);

    //     effect(); // efface le contenu précédent

    //     lcd->Set_Text_Back_colour(colour_back);
    //     lcd->Set_Text_colour(colour_txt);
    //     lcd->Set_Text_Size(4);

    //     z = 0;
    //     if ((value < 1000) && (value > 100)) { z = 20; }
    //     lcd->Print_Number_Int(value, x0 + 10 + z, y0 + 18, nb_chiffres, ' ', 10);

    //     lcd->Set_Text_colour(colour_txt);
    //     lcd->Set_Text_Size(2);
    //     lcd->Print_String(txt_unit, x0 + dx - 40, y0 + 35); // ex : mm, kHz, etc...

    //     if (((v_min != 0) && (v_max != 0)) && m_renderMinMax)
    //     {
    //         // affiche valeurs min et max en haut à droite
    //         lcd->Set_Text_colour(colour_name /*GRAY_LIGHT*/);
    //         lcd->Set_Text_Size(1);

    //         z = 0;
    //         if (v_max < 1000) { z = 5; }
    //         lcd->Print_Number_Int(v_max, x0 + dx - 30 + z, y0 + 9, 3, ' ', 10);

    //         z = 0;
    //         if (v_min < 1000) { z = 5; }
    //         lcd->Print_Number_Int(v_min, x0 + dx - 30 + z, y0 + 17, 3, ' ', 10);
    //     }
    // }
    void Sticker::Show_float(float value, uint8_t nb_chiffres, uint8_t nb_dec)
    {
        drawTitle();
        drawMinMaxFloat();
        drawUnit();
        clearMainArea();

        //uint16_t z = 0;
        lcd->Set_Text_Back_colour(colour_back);
        lcd->Set_Text_colour(colour_txt);
        lcd->Set_Text_Size(4);

        //RAPPEL:	Print_Number_Float(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler);
        lcd->Print_Number_Float(value, nb_dec, x0 + 20, y0 + 21, '.', nb_chiffres, ' ');
    }
    // void Sticker::Show_float(float value, uint8_t nb_chiffres, uint8_t nb_dec, char txt_unite_i[3], float v_min, float v_max)
    // {
    //     uint16_t z;
	// 	SetUnit(txt_unite_i);

    //     effect(); // efface le contenu précédent

    //     lcd->Set_Text_Back_colour(colour_back);
    //     lcd->Set_Text_colour(colour_txt);
    //     lcd->Set_Text_Size(4);

    //     //RAPPEL:	Print_Number_Float(double num, uint8_t dec, int16_t x, int16_t y, uint8_t divider, int16_t length, uint8_t filler);
    //     lcd->Print_Number_Float(value, nb_dec, x0 + 20, y0 + 18, '.', nb_chiffres, ' ');

    //     lcd->Set_Text_colour(colour_txt);
    //     lcd->Set_Text_Size(2);
    //     lcd->Print_String(txt_unit, x0 + dx - 40, y0 + 35); // ex : deg, hPa, mm, kHz ...

    //     if (((v_min != 0) || (v_max != 0)) && m_renderMinMax)
    //     {
    //         // affiche valeurs min et max en haut à droite
    //         //lcd->Set_Text_Back_colour(BLACK);
    //         lcd->Set_Text_colour(colour_name /*GRAY_LIGHT*/);
    //         lcd->Set_Text_Size(1);

    //         z = 0;
    //         if (v_max < 10){ z = 6; }
    //         lcd->Print_Number_Float(v_max, 1, x0 + dx - 30 + z, y0 + 9, '.', 3, ' ');

    //         z = 0; if (v_min < 10) {z = 6; }
    //         lcd->Print_Number_Float(v_min, 1, x0 + dx - 30 + z, y0 + 17, '.', 3, ' ');
    //     }
    // }
    void Sticker::Show_HEX(uint32_t value){};
    void Sticker::Show_text(char txt_i[10])
    {
        for (int i=0; i<10; i++) {text[i]=txt_i[i];}
        text[10]='\0';  // zero terminal
        
        //effect();
        drawTitle();
        clearInsideFrame();

        lcd->Set_Text_Back_colour(colour_back);
        lcd->Set_Text_colour(colour_txt);
        lcd->Set_Text_Size(3);
        lcd->Print_String(text, x0+5,y0+18); 
    };
    void Sticker::Show_string(String txt_i)
    {
        //effect();
        drawTitle();
        clearInsideFrame();

        lcd->Set_Text_Back_colour(colour_back);
        lcd->Set_Text_colour(colour_txt);
        lcd->Set_Text_Size(3);
        lcd->Print_String(txt_i, x0 + 8, y0 + 24);
    };

    // void Sticker::traceBorder()
    // {
    //     lcd->Set_Draw_color(colour_frame);
    //     lcd->Draw_Rectangle(x0, y0 + 5, x0 + dx, y0 + dy);
    // }
    // void Sticker::showName()
    // {
    //     lcd->Set_Text_Back_colour(colour_back);
    //     lcd->Set_Text_colour(colour_name);
    //     lcd->Set_Text_Size(2);
    //     lcd->Print_String(name, x0, y0);
    // }
    // void Sticker::effect()
    // {
    //     lcd->Set_Draw_color(colour_back);
    //     lcd->Fill_Rectangle(x0 + 1, y0 + 6, x0 + dx - 1, y0 + dy - 1);
    //     showName();
    // }

    void Sticker::drawTitle()
    {
        if( m_renderTitle)
        {        
            lcd->Set_Draw_color(colour_back);
            lcd->Fill_Rectangle(x0, y0, x0 + dx, y0 + dy);
            lcd->Set_Draw_color(colour_frame);
            lcd->Draw_Rectangle(x0, y0, x0 + dx, y0 + dy);
            lcd->Set_Text_Back_colour(colour_back);
            lcd->Set_Text_colour(colour_name);
            lcd->Set_Text_Size(2);
            lcd->Print_String(name, x0, y0+2);
            m_renderTitle = false;
        }
    }
    void Sticker::clearInsideFrame()
    {
         lcd->Set_Draw_color(colour_back);
         lcd->Fill_Rectangle(x0 + 1, y0 + 17, x0 + dx - 1, y0 + dy - 1);
    }
    void Sticker::clearMainArea()
    {
        lcd->Set_Draw_color(colour_back);
        lcd->Fill_Rectangle(x0 + 1, y0 + 17, x0 + dx - 42, y0 + dy - 1);
    }
    void Sticker::clearMaxMinArea()
    {
        lcd->Set_Draw_color(colour_back);
        lcd->Fill_Rectangle(x0 + dx - 30, y0 + 2, x0 + dx - 1, y0 + 18);
    }
    void Sticker::drawMinMaxInt()
    {
        if (m_renderMinMax)
        {
            clearMaxMinArea();

            // affiche valeurs min et max en haut à droite
            lcd->Set_Text_Back_colour(colour_back);
            lcd->Set_Text_colour(colour_name /*GRAY_LIGHT*/);
            lcd->Set_Text_Size(1);

            uint16_t z = 0;
            if (vmax_int < 1000) { z = 5; }
            lcd->Print_Number_Int(vmax_int, x0 + dx - 30 + z, y0 + 9, 3, ' ', 10);

            z = 0;
            if (vmin_int < 1000) { z = 5; }
            lcd->Print_Number_Int(vmin_int, x0 + dx - 30 + z, y0 + 17, 3, ' ', 10);

            m_renderMinMax = false;
        }
    }
    void Sticker::drawMinMaxFloat()
    {
        if ( m_renderMinMax)
        {
            clearMaxMinArea();
            // affiche valeurs min et max en haut à droite
            //lcd->Set_Text_Back_colour(BLACK);
            lcd->Set_Text_Back_colour(colour_back);
            lcd->Set_Text_colour(colour_name /*GRAY_LIGHT*/);
            lcd->Set_Text_Size(1);

            uint16_t z = 0;
            if (vmax_float < 10){ z = 6; }
            lcd->Print_Number_Float(vmax_float, 1, x0 + dx - 30 + z, y0 + 8, '.', 3, ' ');

            z = 0; if (vmin_float < 10) {z = 6; }
            lcd->Print_Number_Float(vmin_float, 1, x0 + dx - 30 + z, y0 + 17, '.', 3, ' ');

            m_renderMinMax = false;
        }
    }
    void Sticker::drawUnit()
    {
        if(m_renderUnit)
        {
            lcd->Set_Draw_color(colour_back);
            lcd->Fill_Rectangle(x0 + dx - 40, y0 + 35, x0 + dx - 2, y0 + dy - 2);

            lcd->Set_Text_Back_colour(colour_back);
            lcd->Set_Text_colour(colour_txt);
            lcd->Set_Text_Size(2);
            lcd->Print_String(txt_unit, x0 + dx - 40, y0 + 35); // ex : deg, hPa, mm, kHz ...
            m_renderUnit = false;
        }
    }
};

#undef STICKER__CPP