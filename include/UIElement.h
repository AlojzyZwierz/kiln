#pragma once
#include <TFT_eSPI.h> // Include the TFT_eSPI library
class UIElement {
    public:
        virtual void render(TFT_eSprite& tft) = 0;
        void setVisible(bool v) { visible = v; }
        bool isVisible() const { return visible; }
    
    protected:
        bool visible = false;
    };
    