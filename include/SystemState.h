#pragma once
#include <functional>
#include <Arduino.h>
enum class SystemMode
{
    Idle,
    Firing,
    Edit,
    Error
};

class SystemState
{
public:
    static SystemState &get()
    {
        static SystemState instance;
        return instance;
    }

    void setMode(SystemMode newMode)
    {
        if (mode != newMode)
        {
            mode = newMode;
            if (onModeChange)
                onModeChange(mode);
        }
        if (newMode != SystemMode::Firing)
        {
            digitalWrite(SSR_PIN, LOW);
        }
    }
    SystemMode getMode() const { return mode; }

    bool isLocked() const { return mode == SystemMode::Firing; }
    std::function<void(SystemMode)> onModeChange;
    std::function<void(bool)> onArmedChange;
    void setCooling(bool isCooling)
    {
        cooling = isCooling;
    }
    bool isCooling() const
    {
        return cooling;
    }
    void setArmed(bool isArmed, int delayTimeH, int curveIndex)
    {
        delayed = isArmed;
        delayedCurveIndex = curveIndex;
        if (delayed)
        {
            delayedTime = millis() + delayTimeH * 3600000; // przeliczamy godziny na milisekundy
        }
    }
    bool isArmed() const
    {
        return delayed;
    }
    unsigned long getDelayedTime() const
    {
        return delayedTime;
    }
    int getDelayedCurveIndex() const
    {
        return delayedCurveIndex;
    }
    void setDelayedCurveIndex(int index)
    {
        delayedCurveIndex = index;
    }
    bool isDelayElapsed() const
    {
        return delayed && (uint32_t)(millis()> delayedTime) ;
    }
    uint32_t getArmedRemainingMs() const {
    if (!delayed) return 0;
    uint32_t elapsed =   delayedTime - millis() ;
    return elapsed < 0 ? 0 :  elapsed;
}

private:
    SystemState() = default;
    SystemMode mode = SystemMode::Idle;
    bool cooling = false; // czy jest gorący i chłodzi się po wypale
    bool delayed = false;
    unsigned long delayedTime = 0;
    int delayedCurveIndex = -1; // indeks krzywej, która ma być uruchomiona po opóźnieniu
};