#ifndef GUI_RENDERER_H
#define GUI_RENDERER_H

#include <TFT_eSPI.h>
#include "ArrowButton.h"
#include "TextButton.h"
#include "TemperatureSensor.h"
#include "CurveManager.h"
#include "Graphrenderer.h"
#include "CurveSelector.h"
#include <Fonts/GFXFF/FreeSans12pt7b.h>
#include "fonts.h"
#include "GUIMode.h"
#include <vector>
#include <memory>
#include <ProcessController.h>
#include "UILabel.h"
#include "EditCircle.h"
#include "Utils.h"
#include "CurveManager.h"
#include "StorageManager.h"
#include "FakeFurnace.h"

class Clickable;

class GUIRenderer {
public:
   //           GUIRenderer(TFT_eSPI& display, TemperatureSensor& sensor, CurveSelector& cs, CurveManager& cm);
   GUIRenderer(TFT_eSPI& display, TemperatureSensor& sensor, CurveSelector& cs, CurveManager& cm, FakeFurnace& f);

    void render();               // Rysuje cały GUI
    void handleTouch(int x, int y); // Obsługa kliknięć
    void setMode(GUIMode newMode) { 
        CurrentMode = newMode; 
        setupUIFormodes(CurrentMode); // Ustawia widoczność przycisków
        Serial.println("mode Set: " + String(static_cast<int>(CurrentMode)));
    }// Ustawia tryb GUI

private:
    TFT_eSPI& tft;
    TFT_eSprite sprite;
    TemperatureSensor& tempSensor;
    CurveManager& curveManager;
    CurveSelector& curveSelector;
    ArrowButton leftArrow;
    ArrowButton rightArrow;
    TextButton startButton;
    TextButton editButton;
    TextButton stopButton;
    TextButton closeButton;
    TextButton saveButton;
    TextButton endHereButton;
    UILabel temperatureLabel;
    UILabel curveIndexLabel;
    UILabel expectedTempLabel;
    UILabel timeLabel;
    UILabel segmentIndexLabel;
    EditCircle editCircle;
    GraphRenderer graphRenderer; 
    void drawHeader(); // Rysuje temperaturę i nr krzywej
    std::vector<Clickable*> clickables;
    std::vector<UIElement*> uiElements; // Wektor elementów UI
    GUIMode CurrentMode = GUIMode::Idle;
    void setupUIFormodes(GUIMode mode);
    FakeFurnace& furnace; 
};

#endif
