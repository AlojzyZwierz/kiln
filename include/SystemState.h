#pragma once
enum class SystemMode {
    Idle,
    Firing,
    Edit,
    Error
};

class SystemState {
public:
    static SystemState& get() {
        static SystemState instance;
        return instance;
    }
std::function<void(SystemMode)> onModeChange;
   void setMode(SystemMode newMode) {
    if (mode != newMode) {
        mode = newMode;
        if (onModeChange) onModeChange(mode);
    }
}
    SystemMode getMode() const { return mode; }

    bool isLocked() const { return mode == SystemMode::Firing; }

private:
    SystemState() = default;
    SystemMode mode = SystemMode::Idle;
    
};