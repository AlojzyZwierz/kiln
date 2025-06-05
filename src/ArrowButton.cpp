#include "ArrowButton.h"

ArrowButton::ArrowButton(int x, int y, int size, ArrowDirection dir, uint16_t color)
    :  posX(x), posY(y), size(size), direction(dir), arrowColor(color) {
    active = true;
    highlighted = false;
}
/*
void ArrowButton::setCallback(std::function<void()> cb) {
    callback = cb;
}
*/
void ArrowButton::render(TFT_eSprite& tft) {
    int half = size / 2;

    if (direction == ArrowDirection::Left) {
        tft.fillTriangle(
     posX + size, posY,               // góra prawa
        posX, posY + half,               // środek lewa
        posX + size, posY + size,   
            arrowColor
        );
    } else {
        tft.fillTriangle(
            posX + size, posY + half,
            posX, posY,
            posX, posY + size,
            arrowColor
        );
    }

    if (highlighted) {
        tft.drawRect(posX - 2, posY - 2, size + 4, size + 4, COLOR_RED_DOT);
    }
}

bool ArrowButton::wasClicked(int x, int y) {
    //Serial.println("___Arrow clicked: " + String (x) + " " + String(y));
    if (!active) return false;

    if (x >= posX && x <= posX + size &&
        y >= posY && y <= posY + size) {
        return true;
    }
    return false;
}


