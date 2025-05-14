#include "SoundManager.h"


void SoundManager::playFanfare() {
    const int melody[] = { 440, 494, 523, 587 };
    const int duration[] = { 200, 200, 200, 400 };

    for (int i = 0; i < 4; i++) {
        tone(BUZZERPIN, melody[i], duration[i]);
        delay(duration[i]);
    }
    noTone(BUZZERPIN);
}

void SoundManager::playError() {
    for (int i = 0; i < 3; i++) {
        tone(BUZZERPIN, 300, 200);
        delay(250);
    }
    noTone(BUZZERPIN);
}

void SoundManager::beep(int freq, int duration) {
    tone(BUZZERPIN, freq, duration);
    delay(duration);
    noTone(BUZZERPIN);
}