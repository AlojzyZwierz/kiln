#pragma once
#include <stdarg.h>
#include <stdio.h>
#include "UIElement.h"

// Mała ramka z trzema liniami tekstu jedna pod drugą, zaczepiona lewym górnym rogiem.
class InfoBox : public UIElement
{
public:
    static constexpr int LINES = 3;
    static constexpr int LEN   = 24;

    InfoBox(int x, int y, uint16_t color,
            uint8_t size0 = 2, uint8_t size1 = 1, uint8_t size2 = 1, int gap = 1)
        : x(x), y(y), color(color), gap(gap)
    {
        sizes[0] = size0; sizes[1] = size1; sizes[2] = size2;
        for (auto &l : lines) l[0] = '\0';
    }

    // printf-owy zapis do wybranej linii, np. setLine(0, "%.0f C", temp)
    void setLine(int i, const char *fmt, ...)
    {
        if (i < 0 || i >= LINES) return;
        va_list args;
        va_start(args, fmt);
        vsnprintf(lines[i], LEN, fmt, args);
        va_end(args);
    }

    void setPosition(int nx, int ny) { x = nx; y = ny; }

    void render(TFT_eSprite &tft) override
    {
        if (!visible) return;
        tft.setTextColor(color);
        tft.setTextDatum(TL_DATUM);
        int yy = y;
        for (int i = 0; i < LINES; i++)
        {
            tft.setTextSize(sizes[i]);
            tft.drawString(lines[i], x, yy);
            yy += tft.fontHeight() + gap;
        }
    }

private:
    int x, y;
    uint16_t color;
    int gap;
    uint8_t sizes[LINES];
    char lines[LINES][LEN];
};