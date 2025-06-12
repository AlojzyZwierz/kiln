#pragma once
#include <vector>
#include "UIElement.h"
#include "TextButton.h"
#include <functional>
#include <WiFi.h>
#include "UILabel.h"
#include "SettingsManager.h"
#include "StorageManager.h"
#include <TFT_eSPI.h> // Include the TFT_eSPI library
#include "ColorPalette.h"
#include "TemperatureSensor.h"

enum class ModalMode
{
    None,
    Settings,
    Error,
    Info, 
    Confirmation
};

class Modal
{
public:
    Modal();
    void show(ModalMode mode, const String &extra = " ");
    void hide();

    void render(TFT_eSprite &sprite);
    bool handleClick(int x, int y);

    bool isVisible() const { return visible; }
    std::function<void()> onClose;
    void setCJTemp(float temp) { 
        cjTemp = temp;
        Serial.println("setCJTemp: " + String(cjTemp));
    }

private:
    void buildSettings();
    void updateFromCurrentEntry();
    void buildError(const String &errorMessage = "An error occurred. Please try again.");
    // void buildInfo();

    bool visible = false;
    ModalMode currentMode = ModalMode::None;
    String title;

    std::vector<UIElement *> uiElements;
    std::vector<TextButton *> clickables;

    // Można je zdefiniować raz i reużywać
    TextButton okButton;
    TextButton closeButton;

    TextButton nextButton;
    TextButton prevButton;
    TextButton plusButton;
    TextButton minusButton;

    UILabel valueLabel;     // (opcjonalnie: etykieta do wyświetlania informacji)
    UILabel entryNameLabel; // (opcjonalnie: etykieta tytułu)
    UILabel ipLabel;
    UILabel cjTempLabel; // Etykieta do wyświetlania temperatury cold junction

    // (opcjonalnie: komunikat dla okna info)
    String infoMessage;

    float cjTemp = 0.0f; // temperatura cold junction
};
