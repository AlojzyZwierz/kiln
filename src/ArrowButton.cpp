#include "ArrowButton.h"

ArrowButton::ArrowButton(TFT_eSprite& display, int x, int y, int size, ArrowDirection dir, uint16_t color)
    : tft(display), posX(x), posY(y), size(size), direction(dir), arrowColor(color) {
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
            posX, posY + half,
            posX + size, posY,
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
        tft.drawRect(posX - 2, posY - 2, size + 4, size + 4, TFT_YELLOW);
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


