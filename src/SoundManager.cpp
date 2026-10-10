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

  void SoundManager::playInTheHallOfTheMountainKing() {
  // Częstotliwości nut (Hz)
  // Motyw zaczyna się od E, potem sekwencja chromatyczna w górę
  // Oryginalna tonacja: e-moll, motyw od mi

  // Czas trwania (ms)
  int tempo = 400; // ćwierćnuta ~400ms, dostosuj do gustu

  // Definicja nut: {częstotliwość, czas trwania}
  // Pierwsza fraza (13 dźwięków):
  // E4 - F#4 - G4 - A4 - C5 - E5 - D5 - C5 - A4 - C5 - D5 - pauza - D5
  // (klasyczny układ Griega, pierwsza fraza tematu)

  struct Note {
    int freq;   // 0 = pauza
    int dur;    // w ms
  };

  Note phrase[] = {
    {330, tempo},    // E4
    {370, tempo},    // F#4
    {392, tempo},    // G4
    {440, tempo},    // A4
    {523, tempo},    // C5
    {659, tempo},    // E5
    {587, tempo},    // D5
    {523, tempo},    // C5
    {440, tempo},    // A4
    {523, tempo},    // C5
    {587, tempo},    // D5
    {0,   tempo/2},  // pauza (ósemka)
    {587, tempo * 3 / 2}, // D5 (ćwierćnuta z kropką)
  };

  int count = sizeof(phrase) / sizeof(phrase[0]);

  for (int i = 0; i < count; i++) {
    if (phrase[i].freq == 0) {
      noTone(BUZZERPIN);
    } else {
      tone(BUZZERPIN, phrase[i].freq, phrase[i].dur);
    }
    // Krótka przerwa między nutami dla artykulacji
    delay(phrase[i].dur + 30);
    noTone(BUZZERPIN);
  }
}
void SoundManager::playMountainKing() {

    const int shortNote = 80;
    const int longNote  = 180;

    const int melody[] = {
        247, // B
        262, // C
        294, // D
        330, // E
        370, // F#
        294, // D
        370, // F#  <- dłuższa

        349, // F
        294, // D
        349, // F   <- dłuższa

        311, // Eb
        262, // C
        311  // Eb  <- dłuższa
    };

    const int durations[] = {
        shortNote,
        shortNote,
        shortNote,
        shortNote,
        shortNote,
        shortNote,
        longNote,

        shortNote,
        shortNote,
        longNote,

        shortNote,
        shortNote,
        longNote
    };

    for (int i = 0; i < 13; i++) {
        beep(melody[i], durations[i]);
    }
}