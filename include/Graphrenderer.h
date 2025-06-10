#ifndef GRAPH_RENDERER_H
#define GRAPH_RENDERER_H

#include <TFT_eSPI.h>
#include <CurveManager.h>
#include "MeasurementManager.h"
#include <fonts.h>
// class GUIMode;
#include "SystemState.h"
#include "ProcessController.h"
#include "FakeFurnace.h"
#include <CurveManager.h>
#include "ColorPalette.h"

class GraphRenderer
{
public:
  GraphRenderer(TFT_eSprite &tftSprite, CurveManager &cm, TemperatureSensor &ts);
  void drawMeasurements(unsigned long totalTime);
  // void render(const Curve& curve);
  void render();
  // void render(TFT_eSPI& display);
  void drawLineWithThickness(TFT_eSprite &tftSprite, int x0, int y0, int x1, int y1, uint16_t color, int thickness);

private:
  // TFT_eSPI& tft;
  TFT_eSprite &sprite;
  float timeRatio = 1.0f;
  float tempRatio = 1.0f;
  CurveManager &curveManager;
  // FakeFurnace& furnace ; // Get the singleton instance of FakeFurnace
  TemperatureSensor &temperatureSensor;
  const float activeGraphArea = 0.8f;
  int currentTempPosX = 0;
  int currentTempPosY = 0;

  unsigned long calculateTotalTime(const Curve &curve);
  void drawGrid(unsigned long totalTime);
  void drawCurve(const Curve &curve, int selectedSegment = -1);
  void drawTimeLabels(unsigned long totalTime);
  void drawTempLabels();
  void drawCurrentTempDot(float temp, unsigned long totalTime);
  void drawThickLine(TFT_eSprite &tft, int x0, int y0, int x1, int y1, uint16_t color, int thickness);
};

#endif
