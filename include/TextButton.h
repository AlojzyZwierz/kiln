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
    TextButton(const String &label, int x, int y, int w, int h,
               uint16_t bgColor, uint16_t textColor, int size = 1);
    void setTextSize(int size);
    // w sekcji private, po fontColor:
    int textSize;

    void render(TFT_eSprite &tft);
    bool wasClicked(int tx, int ty) override;

    void setActive(bool isActive);
    bool getActive() const;

    void setHighlighted(bool state);
    bool isHighlighted() const;
    void setText(const String &newText);

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
