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

enum class ModalMode {
    None,
    Settings,
    Error,
    Info
};

class Modal {
public:
    Modal() ;
    void show(ModalMode mode, const String& extra = "");
    void hide();

    void render(TFT_eSprite& sprite);
    bool handleClick(int x, int y);

    bool isVisible() const { return visible; }
    std::function<void()> onClose;

private:
    void buildSettings();
    void updateFromCurrentEntry();
    //void buildError();
    //void buildInfo();

    bool visible = false;
    ModalMode currentMode = ModalMode::None;
    String title;

    std::vector<UIElement*> uiElements;
    std::vector<TextButton*> clickables;

    // Można je zdefiniować raz i reużywać
    TextButton okButton;
    TextButton closeButton;

    TextButton nextButton;
    TextButton prevButton;
    TextButton plusButton;
    TextButton minusButton;

    UILabel valueLabel; // (opcjonalnie: etykieta do wyświetlania informacji)
    UILabel entryNameLabel; // (opcjonalnie: etykieta tytułu)
    UILabel ipLabel;

    // (opcjonalnie: komunikat dla okna info)
    String infoMessage;
};
