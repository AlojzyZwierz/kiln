#pragma once

#include <Arduino.h>

class SoundManager
{
public:
    static void begin(int pin);
    static void playFanfare();
    static void playError();
    static void beep(int freq = 1000, int duration = 100);
    static void playTone(int freq, int duration_ms);
    static void siren();

    static void click();
    static void wobbleStartSound();
private:
};