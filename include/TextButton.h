#ifndef TEXT_BUTTON_H
#define TEXT_BUTTON_H

#include <TFT_eSPI.h>
#include <functional>
#include "Clickable.h"
#include "ColorPalette.h"
#include "fonts.h"

class TextButton : public Clickable
{
public:
    TextButton(
        const String &label,
        int x, int y, int w, int h,
        uint16_t bgColor = COLOR_BG,
        uint16_t textColor = COLOR_BLACK);

    void render(TFT_eSprite &tft);
    bool wasClicked(int tx, int ty) override;

    void setActive(bool isActive);
    bool getActive() const;

    void setHighlighted(bool state);
    bool isHighlighted() const;

private:
    // TFT_eSprite& tft;
    String text;
    int posX, posY, width, height;
    uint16_t backgroundColor;
    uint16_t fontColor;
    bool highlighted;
    // std::function<void()> onClick;
};

#endif
