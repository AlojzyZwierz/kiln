#pragma once
#define BUZZERPIN 27
#include <Arduino.h>

class SoundManager
{
public:
    static void begin(int pin);
    static void playFanfare();
    static void playError();
    static void beep(int freq = 1000, int duration = 100);

private:
};