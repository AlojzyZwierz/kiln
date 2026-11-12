#pragma once
#include <TFT_eSPI.h>

// Nazwane czcionki dostępne w całym projekcie
enum class NamedFont : uint8_t {
    BITMAP1,
    BITMAP2,
    BITMAP3,
    GFXFF_SMALL,
    GFXFF_MEDIUM,
    GFXFF_LARGE,
    GFXFF_SMALL_BOLD,
    GFXFF_MEDIUM_BOLD,
    GFXFF_LARGE_BOLD,
    GFXFF_FONT_MONO
};

// Wewnętrzny opis: który system czcionek + co ustawić
struct FontSpec {
    bool          isGfx;      // true => GFXFF, false => bitmap
    uint8_t       bitmapFont; // dla bitmap: 1,2,3...
    const GFXfont* gfxFont;   // dla GFXFF
    uint8_t       size;   // wielkość tekstu (dla bitmap)
};

// Jedno miejsce mapowania NamedFont -> FontSpec
inline FontSpec resolveFont(NamedFont nf)
{
    switch (nf) {
        case NamedFont::BITMAP1:            return { false, 1, nullptr };
        case NamedFont::BITMAP2:            return { false, 2, nullptr };
        case NamedFont::BITMAP3:            return { false, 4, nullptr }; // Font 4 = "Font 3" w TFT_eSPI? patrz uwaga niżej

        case NamedFont::GFXFF_SMALL:        return { true, 0, &FreeSans9pt7b  };
        case NamedFont::GFXFF_MEDIUM:       return { true, 0, &FreeSans12pt7b };
        case NamedFont::GFXFF_LARGE:        return { true, 0, &FreeSans18pt7b };

        case NamedFont::GFXFF_SMALL_BOLD:   return { true, 0, &FreeSansBold9pt7b  };
        case NamedFont::GFXFF_MEDIUM_BOLD:  return { true, 0, &FreeSansBold12pt7b };
        case NamedFont::GFXFF_LARGE_BOLD:   return { true, 0, &FreeSansBold18pt7b };
        case NamedFont::GFXFF_FONT_MONO:          return { true, 0, &FreeMono9pt7b };
    }
    return { false, 1, nullptr }; // fallback
}

// Nakłada czcionkę na sprite. textSize działa tylko dla bitmap.
inline void applyFont(TFT_eSprite &tft, NamedFont nf, uint8_t textSize = 1)
{
    FontSpec fs = resolveFont(nf);
    if (fs.isGfx) {
        tft.setTextSize(1);
        tft.setFreeFont(fs.gfxFont);
    } else {
        tft.setFreeFont(nullptr);
        tft.setTextFont(fs.bitmapFont);
        tft.setTextSize(fs.size);
    }
}