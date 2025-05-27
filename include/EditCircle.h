#pragma once

#include <TFT_eSPI.h> // Include the TFT_eSPI library
#include <Clickable.h>
#include <CurveManager.h>
#include <cmath>

class EditCircle : public Clickable {
public:
    EditCircle( int x, int y, int radius, CurveManager& cm) ;

    void render(TFT_eSprite& tft) override;
    bool wasClicked(int x, int y) override;
    bool handleClick(int x, int y) override;
    void setCallback(std::function<void()> cb) { callback = cb;     }
    //void setSegmentIndex(int index) {         segmentIndex = index;      }
    //int getSegmentIndex() const { return segmentIndex; }

    private:
    //Curve curve;
    CurveManager& curveManager;
    //int segmentIndex = 0;
    //TFT_eSprite& tft;
    int posX, posY;
    int radius;
    uint16_t circleColor;
    float mapValue(float val, float inMin, float inMax, float outMin, float outMax);
    
};