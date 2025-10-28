
# Kiln Controller - Advanced Ceramic Kiln Control System

![License](https://img.shields.io/badge/License-GPLv3-blue.svg)
![Platform](https://img.shields.io/badge/Platform-ESP32-orange.svg)
![PlatformIO](https://img.shields.io/badge/PlatformIO-Compatible-brightgreen.svg)

An advanced, feature-rich controller for ceramic kilns built on ESP32-WROOM with touchscreen interface and web monitoring capabilities.

## ⚠️ Safety Warning
**This is a DIY project. Use at your own risk!** 
Ceramic kilns can be dangerous and may cause fire, electric shock, or property damage. Always implement external safety measures and never leave the kiln unattended during operation. The author takes no responsibility for any damages or injuries.

## ✨ Features

### Core Functionality
- **Multi-segment firing programs** - Create complex firing curves with heating, holding, and controlled cooling ramps (default: 25 segments)
- **SPIFFS storage** - Save multiple firing programs with persistent storage
- **Power failure recovery** - Automatically resumes firing after power interruption
- **Hot start capability** - Start firing mid-curve when kiln is already preheated

### Safety & Reliability
- **Multiple safety features** - Temperature limits, error detection, thermal runaway protection
- **PID control** - Custom-written, highly effective PID controller with manual calibration
- **Long-term stability** - Auto-restart after extended runtime to prevent `millis()` overflow
- **Cycle-based power control** - PWM-like control with configurable cycle length to protect mechanical relays

### User Interface
- **Touchscreen interface** - 320x240 color TFT with intuitive controls
- **Edit Circle** - Innovative circular interface for parameter adjustment:
  - Top/Bottom segments: Temperature adjustment
  - Left/Right segments: Time adjustment
  - Distance from center controls adjustment magnitude
- **Real-time graphing** - Live firing curves and progress display
- **Audible feedback** - Interaction sounds, error alerts, startup fanfare, and completion celebration

### Connectivity & Monitoring
- **Web interface** - View firing progress and measurement lists via integrated web server
- **Dual WiFi modes** - Access Point mode or connect to existing WiFi
- **Web configuration** - Set WiFi credentials through web interface

### Advanced Features
- **Energy cost tracking** - Calculate firing costs based on kiln power and energy rates
- **Default bisque firing curve** - Pre-configured for common ceramic needs
- **Extensible design** - Easy to modify and extend

## 🛠️ Hardware Requirements

### Main Components
- **ESP32-WROOM** development board
- **TFT Touchscreen** - 320x240 resolution with XPT2046 touch controller
- **MAX31856** - Thermocouple amplifier (supports K, S, B type thermocouples)
- **Solid State Relay (SSR)** - For kiln power control
- **Buzzer** - For audible feedback

### Pin Configuration
```cpp
// TFT Display
TFT_MISO=12, TFT_MOSI=13, TFT_SCLK=14, TFT_CS=15, TFT_DC=2, TFT_RST=4

// MAX31856 Thermocouple
MAX_CS=5, MAX_CLK=18, MAX_MISO=19, MAX_MOSI=23

// Touch Controller (XPT2046)
XPT2046_IRQ=36, XPT2046_MOSI=32, XPT2046_MISO=39, XPT2046_CLK=25, XPT2046_CS=33

// Other
SSR_PIN=16, BUZZERPIN=22
📋 Software Requirements
Visual Studio Code with PlatformIO extension

ESP32 framework (included in PlatformIO)

🚀 Installation
For Beginners
Install Visual Studio Code

Install PlatformIO IDE extension

Clone this repository

Open project in PlatformIO

Connect your ESP32 via USB

Build and upload the project

The controller will create default configuration files automatically on first run

For Advanced Users
Clone the repository

Open in PlatformIO

Review platformio.ini for build flags and dependencies

Modify pin assignments in build flags if needed

Build and upload

Dependencies (automatically handled by PlatformIO)
TFT_eSPI

SPIFFS

Adafruit_MAX31856

ESPAsyncWebServer

AsyncTCP

XPT2046_Touchscreen

🎮 Usage
Basic Operation
Power on the controller

Use touchscreen to navigate

Select or create firing programs using the Edit Circle interface

Start firing process

Monitor progress via touchscreen or web interface

Edit Circle Interface
Temperature Adjustment: Tap top/bottom segments

Time Adjustment: Tap left/right segments

Fine/Coarse Control: Distance from center controls adjustment magnitude

Web Interface
Connect to the controller's WiFi access point

Access http://192.168.4.1 to view:

Real-time firing progress

Temperature measurements

Firing curves

System status

📁 Project Structure
text
kiln/
├── include/         # Header files
├── src/            # Main source code
├── lib/            # Libraries (if any)
├── data/           # Web interface files
└── platformio.ini  # Build configuration
🔧 Configuration
Firing Programs
Default maximum: 25 segments (configurable)

Maximum temperature: 1280°C (configurable)

Programs saved automatically to SPIFFS

Power Control
Cycle-based power management

Configurable cycle length in settings

Suitable for both SSR and mechanical relays

🌐 Web Features
Read-only monitoring for safety reasons

Real-time firing curve display

Measurement history

WiFi configuration interface

⚠️ Limitations & Notes
Display Resolution: 240x320 - graphs use full screen, interface overlays may occasionally become unreadable in complex curves

Program Storage: Limited by SPIFFS capacity

Web Interface: Configuration and program editing disabled for safety

Safety: Software protections only - implement hardware safety measures

📞 Support
Please report issues and questions through GitHub Issues. For security-related concerns, please email the maintainer directly.

📄 License
This project is licensed under the GNU General Public License v3.0 - see the LICENSE file for details.

Happy Firing! 🔥🎨

Remember: Always practice proper kiln safety procedures and never leave operating equipment unattended.
