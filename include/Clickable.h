// Clickable.h
#pragma once

#include <TFT_eSPI.h>
#include <UIElement.h>

class Clickable : public UIElement {
    // Klasa bazowa dla elementów GUI, które można klikać
    // Zawiera metody do rysowania, obsługi kliknięć i zarządzania widocznością
public:
    virtual void render(TFT_eSprite& tft)=0;
    virtual ~Clickable() {}
    //virtual void draw(TFT_eSPI& tft) = 0;
    void onClick(){
        //Serial.println(callback ? "mam callback" : "brak callbacka");
        if (callback) callback();
    }
    virtual bool wasClicked(int touchX, int touchY) = 0;
    virtual bool handleClick(int x, int y) {
        if (wasClicked(x, y)){ onClick();
            Serial.println("Clickable clicked at: " + String(x) + ", " + String(y));
            return true;
        }
        return false;
    }
    void activate(bool setActive){
        active = setActive;
    }
    bool isActive(){
        return active;
    }

    Clickable(bool isActive = true) : active(isActive) {}
    void setCallback(std::function<void()> cb){
        callback = cb;
    }
    protected:
    bool active = true ;
    bool highlighted =false;
    std::function<void()> callback;
};
