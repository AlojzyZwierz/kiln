#pragma once
#include <TFT_eSPI.h>

extern uint16_t uiPalette[256];

enum PaletteColorIndex
{
  COLOR_BG = 63357,
  COLOR_GRID = 54771,
  COLOR_BLACK = 0,
  COLOR_BUTTON = 59193,
  COLOR_MODAL_BG = 59193,
  COLOR_RED_DOT = 60548,
  COLOR_COOLING_LINE = 32159,
  COLOR_EDIT_CIRCLE = 62122
  // ... dodajesz więcej wg potrzeb
};

void buildCustomPalette();
