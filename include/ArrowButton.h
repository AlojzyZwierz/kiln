#ifndef ARROW_BUTTON_H
#define ARROW_BUTTON_H

#include <TFT_eSPI.h>
#include "Clickable.h"
#include "ColorPalette.h"

enum class ArrowDirection
{
    Left,
    Right
};

class ArrowButton : public Clickable
{
public:
    ArrowButton(int x, int y, int size, ArrowDirection dir, uint16_t color);

    void render(TFT_eSprite &tft) override;
    bool wasClicked(int x, int y) override;
    // void draw(TFT_eSPI& tft) override;

    // void setCallback(std::function<void()> cb);

private:
    int posX, posY;
    int size;
    ArrowDirection direction;
    uint16_t arrowColor;
};

#endif
