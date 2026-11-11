#include "TextButton.h"

TextButton::TextButton(
    const String &label,
    int x, int y, int w, int h,
    uint16_t bgColor,
    uint16_t textColor,
    int size) : text(label), posX(x), posY(y), width(w), height(h),
                backgroundColor(bgColor), fontColor(textColor), textSize(size),
                highlighted(false), Clickable(active) {}

void TextButton::render(TFT_eSprite &tft)
{
    uint16_t drawColor = highlighted ? COLOR_GRID : backgroundColor;
    tft.fillRect(posX, posY, width, height, drawColor);
    tft.drawRect(posX, posY, width, height, COLOR_BLACK);
    tft.setTextColor(fontColor);
    tft.setTextSize(textSize);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(text, posX + width / 2, posY + height / 2);
    tft.setTextSize(1);   // sprite to stan globalny, nie zostawiaj dużego rozmiaru następnym elementom
}

bool TextButton::wasClicked(int tx, int ty)
{
    //Serial.println("Button clicked: " + text + "  " + String (posX) + " " + String(posY) + " " + String(width + posX) + " " + String(height + posY));
    if (tx >= posX && tx <= (posX + width) && ty >= posY && ty <= (posY + height))
    {
        //Serial.println("Button down: " + text);
        return true;
    }
    return false;
}

void TextButton::setActive(bool isActive)
{
    active = isActive;
}

bool TextButton::getActive() const
{
    return active;
}

void TextButton::setHighlighted(bool state)
{
    highlighted = state;
}

bool TextButton::isHighlighted() const
{
    return highlighted;
}

void TextButton::setText(const String &newText)
{
    text = newText;
}
/*
void TextButton::onClick(){
    if (callback) callback();
}*/
void TextButton::setTextSize(int size)
{
    textSize = size;
}