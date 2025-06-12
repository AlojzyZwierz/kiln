#include "SoundManager.h"

void SoundManager::playFanfare()
{
    const int melody[] = {440, 494, 523, 587};
    const int duration[] = {200, 200, 200, 400};

    for (int i = 0; i < 4; i++)
    {
        tone(BUZZERPIN, melody[i], duration[i]);
        delay(duration[i]);
    }
    noTone(BUZZERPIN);
}

void SoundManager::playError()
{
    for (int i = 0; i < 3; i++)
    {
        tone(BUZZERPIN, 300, 200);
        delay(250);
    }
    noTone(BUZZERPIN);
}

void SoundManager::beep(int freq, int duration)
{
    tone(BUZZERPIN, freq, duration);
    delay(duration);
    noTone(BUZZERPIN);
}

void SoundManager::playTone(int freq, int duration_ms)
{
    long delay_us = 1000000L / freq / 2;
    long cycles = freq * duration_ms / 1000;
    for (long i = 0; i < cycles; i++)
    {
        digitalWrite(BUZZERPIN, HIGH);
        delayMicroseconds(delay_us);
        digitalWrite(BUZZERPIN, LOW);
        delayMicroseconds(delay_us);
    }
}
void SoundManager::siren()
{
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
}
void SoundManager::click()
{

    {
        playTone(4000, 3); // 4 kHz przez 3 ms
    }
}
void SoundManager::wobbleStartSound()
{
    // 1. Rozchwiane drżenie – nieregularne impulsy
    for (int i = 0; i < 10; i++)
    {
        int freq = random(300, 900); // chaos
        int dur = random(20, 50);
        playTone(freq, dur);
        delay(random(10, 30));
    }

    // 2. Stabilizacja – coraz bliżej jednej częstotliwości
    for (int i = 0; i < 8; i++)
    {
        int freq = 400 + i * 20; // rośnie w stronę stałej
        playTone(freq, 40);
        delay(10);
    }
    delay(100);
    playTone(660, 250); 
}