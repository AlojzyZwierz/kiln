#pragma once
#include <ColorPalette.h> // Include the color palette header
#include <TFT_eSPI.h> // Include the TFT_eSPI library
#include <UIElement.h> // Include the base class header
class UILabel : public UIElement {
    public:
        UILabel(const String& txt, int x, int y, uint16_t color = COLOR_BLACK, int size = 2)
            : text(txt), posX(x), posY(y), textColor(color), textSize(size) {}
    
        void setText(const String& newText) { text = newText; }
        void setColor(uint16_t color) { textColor = color; }
        void setSize(int size) { textSize = size; }
        void setPosition(int x, int y) { posX = x; posY = y; }
    
        void render(TFT_eSprite& tft) override {
            tft.setTextColor(textColor, COLOR_BG); // z tłem
            tft.setTextSize(textSize);
            tft.setCursor(posX, posY);
            tft.print(text);
        }
    
    private:
        String text;
        int posX, posY;
        uint16_t textColor;
        int textSize;
    };
    