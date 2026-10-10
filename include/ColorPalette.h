#pragma once
#include <TFT_eSPI.h>

// Sprite 4-bit: wszędzie przekazujemy INDEKS (0-15), nie wartość RGB565.
constexpr uint8_t UI_PALETTE_SIZE = 16;
extern uint16_t uiPalette[UI_PALETTE_SIZE];   // indeks -> RGB565

enum PaletteColorIndex : uint8_t
{
  COLOR_BLACK = 0,
  COLOR_BG,
  COLOR_GRID,
  COLOR_BUTTON,
  COLOR_MODAL_BG,
  COLOR_RED_DOT,
  COLOR_COOLING_LINE,
  COLOR_EDIT_CIRCLE,
  COLOR_WHITE,
  COLOR_SKIP,
  // 9..15 wolne - dopisuj kolejne tutaj i w buildCustomPalette()
};

void buildCustomPalette();

// Do rysowania BEZPOŚREDNIO na tft (nie na sprite): zamienia indeks na RGB565
inline uint16_t rgb565Of(uint8_t idx) { return uiPalette[idx]; }