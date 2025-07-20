#pragma once

//#define SSR 16 // pin przekaźnika – ustaw wg własnych potrzeb
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
    void setCooling(bool isCooling)
    {
        cooling = isCooling;
    }
    bool isCooling() const
    {
        return cooling;
    }   
private:
    SystemState() = default;
    SystemMode mode = SystemMode::Idle;
    bool cooling = false; // czy jest gorący i chłodzi się po wypale
};