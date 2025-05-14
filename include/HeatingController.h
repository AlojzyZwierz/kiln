#pragma once
#include <Arduino.h>

class HeatingController {
public:
    void update();
    void setRatio(float r);            // wartość PID: 0.0 - 1.0
    float getRatio();  
    void setEnabled(bool on);          // czy grzanie ma działać
    void setCycleTime(unsigned long t); // czas cyklu grzania (config.hc)

    bool isHeating() const;            // czy SSR jest aktualnie załączony

private:
    float ratio = 0.0f;
    unsigned long cycleTime = 10000;   // domyślnie np. 10s
    unsigned long hCycleStartTime = 0;
    bool enabled = false;         // czy grzanie jest włączone
    bool hON = false;         // czy grzanie jest aktualnie włączone
};