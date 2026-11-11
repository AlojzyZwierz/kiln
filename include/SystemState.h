#pragma once
#include <functional>
#include <Arduino.h>
#include <ResumeManager.h>

#define MS_PER_DELAY_HOUR 3600000

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
        armFromMs(isArmed, (uint32_t)delayTimeH * MS_PER_DELAY_HOUR, curveIndex);
    }
    void armFromMs(bool isArmed, uint32_t ms, int curveIndex)
    {
        delayed = isArmed;
        delayedCurveIndex = curveIndex;
        if (delayed)
        {
            delayedTime = millis() + ms;
            ResumeManager::saveArmed(curveIndex, ms);
        }
        else
            ResumeManager::clearArmed();
        if (onArmedChange)
            onArmedChange(delayed);
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
        return delayed && (int32_t)(millis() - delayedTime) >= 0;
    }
    uint32_t getArmedRemainingMs() const
    {
        if (!delayed)
            return 0;
        int32_t rem = (int32_t)(delayedTime - millis());
        return rem > 0 ? (uint32_t)rem : 0;
    }

private:
    SystemState() = default;
    SystemMode mode = SystemMode::Idle;
    bool cooling = false; // czy jest gorący i chłodzi się po wypale
    bool delayed = false;
    unsigned long delayedTime = 0;
    int delayedCurveIndex = -1; // indeks krzywej, która ma być uruchomiona po opóźnieniu
};