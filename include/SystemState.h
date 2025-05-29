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

    void setMode(SystemMode m) { mode = m; }
    SystemMode getMode() const { return mode; }

    bool isLocked() const { return mode == SystemMode::Firing; }

private:
    SystemState() = default;
    SystemMode mode = SystemMode::Idle;
};