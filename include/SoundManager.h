#pragma once
#define BUZZERPIN 22
#include <Arduino.h>

class SoundManager
{
public:
    static void begin(int pin);
    static void playFanfare();
    static void playError();
    static void beep(int freq = 1000, int duration = 100);
    static void playTone(int freq, int duration_ms);
    static void siren()
    {
        for (int i = 500; i < 1000; i++)
        {
            playTone(i, 2);
        }
        for (int i = 1000; i > 500; i--)
        {
            playTone(i, 2);
        }
    }
    static void click()
    {
        playTone( 4000, 3); // 4 kHz przez 3 ms
    }

private:
};