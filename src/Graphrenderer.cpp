#include "GraphRenderer.h"
#include  <CurveManager.h>
GraphRenderer::GraphRenderer(TFT_eSprite& tftSprite, CurveManager& cm, FakeFurnace& f) : sprite(tftSprite), curveManager(cm), furnace(f)  {}



void GraphRenderer::render(GUIMode mode) {
  
  unsigned long totalTime = calculateTotalTime(curveManager.getAdjustedCurve());
  timeRatio = TFT_HEIGHT * activeGraphArea / totalTime;
  tempRatio = TFT_WIDTH / 1300.0f;

  sprite.fillScreen(TFT_WHITE);
  sprite.setTextSize(1);

  drawGrid(totalTime);

  switch (mode)
  {
  case GUIMode::Idle:
  drawCurve(curveManager.getAdjustedCurve());
    break;
  
    case GUIMode::Firing:
    drawCurve(curveManager.getAdjustedCurve(), curveManager.getSegmentIndex());
    //drawMeasurements(curveManager.getMeasurementManager());
    drawTempLabels();
    drawTimeLabels(totalTime);
    drawMeasurements(totalTime);
    drawCurrentTempDot(furnace.getTemperature(), totalTime);
    break;
  
    case GUIMode::Edit:
    drawCurve(curveManager.getAdjustedCurve());

    break;
  
  default:
    break;
  }
}

void GraphRenderer::drawThickLine(TFT_eSprite &tft, int x0, int y0, int x1, int y1, uint16_t color, int thickness) {
  float dx = x1 - x0;
  float dy = y1 - y0;
  float length = sqrt(dx * dx + dy * dy);

  if (length == 0) return;

  // normalna jednostkowa
  float nx = -dy / length;
  float ny = dx / length;

  float halfThickness = thickness / 2.0f;

  // cztery rogi prostokąta reprezentującego "grubą linię"
  int x0a = x0 + nx * halfThickness;
  int y0a = y0 + ny * halfThickness;
  int x0b = x0 - nx * halfThickness;
  int y0b = y0 - ny * halfThickness;

  int x1a = x1 + nx * halfThickness;
  int y1a = y1 + ny * halfThickness;
  int x1b = x1 - nx * halfThickness;
  int y1b = y1 - ny * halfThickness;

  sprite.fillTriangle(x0a, y0a, x1a, y1a, x1b, y1b, color);
  sprite.fillTriangle(x0a, y0a, x0b, y0b, x1b, y1b, color);
}


void GraphRenderer::drawLineWithThickness(TFT_eSprite& tftSprite , int x0, int y0, int x1, int y1, uint16_t color, int thickness) {
  // Oblicz różnice
  float dx = x1 - x0;
  float dy = y1 - y0;
  float length = sqrt(dx * dx + dy * dy) ;

  if (length == 0) return;  // punkt, nie linia

  // Oblicz wektor normalny (prostopadły)
  float nx = -dy / length;
  float ny = dx / length;

  // Rysuj linie równoległe do oryginału, przesunięte wzdłuż normalnej
  for (int i = -thickness / 2; i <= thickness / 2; ++i) {
      int ox = round(nx * i  );
      int oy = round(ny * i );
      sprite.drawLine(x0 + ox, y0 + oy, x1 + ox, y1 + oy, color);
  }
}



unsigned long GraphRenderer::calculateTotalTime(const Curve& curve) {
  unsigned long total = 0;
  for (int i = 0; i < 25; i++) {
    if (curve.elems[i].hTime == 0) break;
    total += curve.elems[i].hTime;
  }
  return total;
}

void GraphRenderer::drawGrid(unsigned long totalTime) {
  for (int i = 1; i < 13; i++) {
    int y = 240 - (int)(i * 100 * tempRatio);
    sprite.drawFastHLine(0, y,TFT_HEIGHT,  TFT_LIGHTGREY);
  }
  for (int i = 0; i < (totalTime / 3600000) + 4; i++) {
    int x = i * 3600000 * timeRatio;
    if (x > 320) break;
    sprite.drawFastVLine(x, 0, TFT_WIDTH, TFT_LIGHTGREY);
  }
}

void GraphRenderer::drawTempLabels() {
  for (int i = 1; i < 13; i++) {
    int y = (int)(i * 100 * tempRatio);
    sprite.setFreeFont(FONT_SMALL);
    sprite.setTextSize(0.2);
    sprite.drawString(String(13 - i * 1), 10, y);
  }
}

void GraphRenderer::drawTimeLabels(unsigned long totalTime) {
  for (int i = 1; i < (totalTime / 3600000) + 4; i++) {
    int x = i * 3600000 * timeRatio;
    if (x > 320) break;
    sprite.setFreeFont(FONT_SMALL);
    sprite.setTextSize(0.5);
    sprite.drawString(String(i), x, 230);
  }
}

void GraphRenderer::drawCurve(const Curve& curve, int selectedSegment) {
  float totalTime = 0;
  float lastX = 0, lastY = 4;

  for (int i = 0; i < 25; i++) {
    if (curve.elems[i].hTime == 0) break;

    totalTime += curve.elems[i].hTime;
    float x = totalTime * timeRatio;
    float y = (curve.elems[i].endTemp ) * tempRatio;
    uint16_t color =  sprite.color565(255, 70 , 120);
    int tempLabelOffset = 0;
    if(curveManager.getSegmentIndex() == i) {
      //sprite.drawLine((int)lastX, 240 - (int)lastY, (int)x, 240 - (int)y, TFT_MAGENTA);
      drawThickLine(sprite, (int)lastX, 240 - (int)lastY, (int)x, 240 - (int)y, color, 3);
      sprite.fillCircle((int)x, 240 - (int)y, 3, color);
    }else{
    //sprite.drawLine((int)lastX, 240 - (int)lastY, (int)x, 240 - (int)y, TFT_BLACK);}
      drawThickLine(sprite, (int)lastX, 240 - (int)lastY, (int)x, 240 - (int)y, TFT_BLACK, 2);
      sprite.fillCircle((int)x, 240 - (int)y, 3, TFT_BLACK);
    }
    if (lastY != y){
      if(curveManager.getSegmentIndex() == i)sprite.setTextColor(color);
    sprite.setFreeFont(FONT_SMALL);
    sprite.setTextSize(0.4);
      sprite.drawString(String(curve.elems[i].endTemp, 0 ), x + 7, 240 - y + 9);
    }
    sprite.setTextColor(TFT_BLACK);
    lastX = x;
    lastY = y;
  }
  sprite.drawLine((int)lastX, 240 - (int)lastY, (int)320, 240 - (int)(20*tempRatio), TFT_BLUE);
}

void GraphRenderer::drawMeasurements(unsigned long totalTime) {
  //const auto& data = MeasurementManager::get().getMeasurements();
  //Serial.println("Measurements no: " + String(MeasurementManager::get().getMeasurements().size()));
  if (MeasurementManager::get().getMeasurements().empty()) return;

  // Przykład uproszczonego rysowania
  
  
  float xScale = TFT_HEIGHT * activeGraphArea * 1000.0f / (totalTime ); 
  float yScale = TFT_WIDTH/1300.0f; 

  for (size_t i = 1; i < MeasurementManager::get().getMeasurements().size(); ++i) {
    int x1 = MeasurementManager::get().getMeasurements()[i - 1].time * xScale;
    int y1 = TFT_WIDTH - MeasurementManager::get().getMeasurements()[i - 1].temp * yScale;
    int x2 = MeasurementManager::get().getMeasurements()[i].time * xScale;
    int y2 = TFT_WIDTH - MeasurementManager::get().getMeasurements()[i].temp * yScale ;

    sprite.drawLine(x1, y1, x2, y2, TFT_RED);
Serial.println("x1: " + String(x1) + " y1: " + String(y1) + " x2: " + String(x2) + " y2: " + String(y2) );
    //sprite.fillCircle(x1, y1, 3, TFT_RED);
    //sprite.fillCircle(x2, y2, 3, TFT_RED);
  }
  //Serial.println("_Measurements drawn: " + String(MeasurementManager::get().getMeasurements().size()));
}

 void GraphRenderer::drawCurrentTempDot(float temp, unsigned long totalTime) {
  float xScale = TFT_HEIGHT *activeGraphArea/ (totalTime ); 
  float yScale = TFT_WIDTH/1300.0f; 
  int x1 = (int)((millis() - ProcessController::get().getCurveStartTime()) * xScale);
  int y1 = TFT_WIDTH - (int)(temp * yScale);
  sprite.fillCircle(x1, y1, 3, TFT_RED);

 }
