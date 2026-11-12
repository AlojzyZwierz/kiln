#pragma once
#include <ColorPalette.h>
#include <TFT_eSPI.h>
#include <UIElement.h>
#include "fonts.h"

class UILabel : public UIElement
{
public:
    UILabel(const String &txt, int x, int y, uint16_t color = COLOR_BLACK,
            NamedFont font = NamedFont::GFXFF_SMALL)
        : text(txt), posX(x), posY(y), textColor(color), font(font) {}

    void setText(const String &newText) { text = newText; }
    void setColor(uint16_t color) { textColor = color; }
    //void setSize(int size) { textSize = size; }
    void setFont(NamedFont f)
    {
        font = f;
    }
    NamedFont getFont() const { return font; }
    void setPosition(int x, int y)
    {
        posX = x;
        posY = y;
    }

    void render(TFT_eSprite &tft) override
    {
        applyFont(tft, font);
        tft.setTextColor(textColor, COLOR_BG); // z tłem
        tft.setCursor(posX, posY);
        tft.print(text);
        //restoreFont(tft);
    }

    int getWidth(TFT_eSprite &tft)
    {
        applyFont(tft, font);
        int width = tft.textWidth(text);
        //restoreFont(tft);
        return width;
    }

private:
/*
    void applyFont(TFT_eSprite &tft)
    {
        if (font)
            tft.setFreeFont(font);
        tft.setTextSize(textSize);
    }
    void restoreFont(TFT_eSprite &tft)
    {
        if (font)
            tft.setFreeFont(FONT_SMALL); // domyślna czcionka całego GUI
        tft.setTextSize(1);
    }
*/
    NamedFont font;
    String text;
    int posX, posY;
    uint16_t textColor;
    //int textSize;
    //const GFXfont *font;   // nullptr = zostaw czcionkę globalną, jak dotąd
};