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

void SoundManager::playTone( int freq, int duration_ms) {
  long delay_us = 1000000L / freq / 2;
  long cycles = freq * duration_ms / 1000;
  for (long i = 0; i < cycles; i++) {
    digitalWrite(BUZZERPIN, HIGH);
    delayMicroseconds(delay_us);
    digitalWrite(BUZZERPIN, LOW);
    delayMicroseconds(delay_us);
  }

}
