#include "ColorPalette.h"

uint16_t uiPalette[UI_PALETTE_SIZE];

static constexpr uint16_t rgb(uint8_t r, uint8_t g, uint8_t b)
{
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

void buildCustomPalette()
{
  for (auto &c : uiPalette) c = 0;

  uiPalette[COLOR_BLACK]        = rgb( 58,  48,  42);
  uiPalette[COLOR_BG]           = rgb(253, 245, 240);
  uiPalette[COLOR_GRID]         = rgb(216, 188, 211);
  uiPalette[COLOR_BUTTON]       = rgb(235, 230, 222);
  uiPalette[COLOR_MODAL_BG]     = rgb(247, 235, 223);
  uiPalette[COLOR_RED_DOT]      = rgb(195, 73, 74);
  uiPalette[COLOR_COOLING_LINE] = rgb( 170, 185, 218);
  uiPalette[COLOR_EDIT_CIRCLE]  = rgb(224, 142, 112);
  uiPalette[COLOR_WHITE]        = rgb(241, 250, 244);
  uiPalette[COLOR_SKIP]         = rgb(121, 70, 101);
}