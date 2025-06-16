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
    delay(50);
    playTone(660, 250); 
    delay(50);
    playTone(380, 330); 
    delay(50);
    playTone(170, 440); 
}
void SoundManager::beep(int note, int duration) {
    if (note == 0) {
      delay(duration);
    } else {
      playTone(note, duration);
    }
    delay(10);  // krótka pauza między nutami
  }
void SoundManager::chiptuneIntro() {
    int melody[] = {
      659, 784, 988, 1319, 1175, 988, 1319, 0, 1319
    };
  
    int durations[] = {
      120, 120, 120, 160, 100, 100, 160, 60, 250
    };
  
    int len = sizeof(melody) / sizeof(melody[0]);
    for (int i = 0; i < len; i++) {
      beep(melody[i], durations[i]);
    }
  }