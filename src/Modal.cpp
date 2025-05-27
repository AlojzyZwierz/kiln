#include "Modal.h"
Modal::Modal()
    : okButton("OK", 30, 180, 80, 40),
      closeButton("X", 270, 30, 25, 25),
      nextButton(">", 230, 110, 40, 40),
      prevButton("<", 60, 110, 40, 40),
      plusButton("+", 150, 50, 40, 40),
      minusButton("-", 150, 170, 40, 40),
      valueLabel("0", 130, 120,     TFT_BLACK, 2),
      entryNameLabel("XXX", 130, 160, TFT_BLACK, 1)
{    
}
void Modal::show(ModalMode mode, const String& extra) {
    Serial.println("Modal::show() called with mode: " + String(static_cast<int>(mode)) + " extra: " + extra);
    
    visible = true;
    currentMode = mode;
    uiElements.clear();
    clickables.clear();
    infoMessage = "";

    switch (mode) {
        case ModalMode::Settings:
            buildSettings();
            break;
        case ModalMode::ConfirmExit:
            //buildConfirmExit();
            break;
        case ModalMode::Info:
            //buildInfo(extra);
            break;
        default:
            break;
    }
}
void Modal::hide() {
    visible = false;
    Serial.println("Modal::hide() called");
    uiElements.clear();
    clickables.clear();
}

void Modal::render(TFT_eSprite& sprite) {
    if (!visible) return;

    sprite.fillRect(20, 20, 280, 200, TFT_LIGHTGREY);
    sprite.drawRect(20, 20, 280, 200, TFT_WHITE);
    sprite.setTextDatum(MC_DATUM);
    sprite.drawString(title, 160, 40);

    for (auto* el : uiElements) {
        el->render(sprite);
    }

    for (auto* btn : clickables) {
        btn->render(sprite);
    }

    // np. info dialog
    if (currentMode == ModalMode::Info && !infoMessage.isEmpty()) {
        sprite.setTextDatum(MC_DATUM);
        sprite.drawString(infoMessage, 160, 100);
    }
}

bool Modal::handleClick(int x, int y) {
    if (!visible) return false;
Serial.println("Modal::handleClick() called at: " + String(clickables.size()) + " " + String(x) + " " + String(y));
    for (auto* el : clickables) {
        if (el->handleClick(x, y)) {
            Serial.println("Clickable element clicked: " + String(el->isVisible()) + " " +  String(x) + " " + String(y));
            return true; // kliknięcie obsłużone
        }
    }

    return false; // żaden przycisk nie został kliknięty
}

void Modal::buildSettings() {
    title = "Settings";
    updateFromCurrentEntry(); // Ustawia aktualną wartość i nazwę wpisu
    // Przyciski np. do zwiększania/zmniejszania wartości
    // lub np. delegacja do `SettingsManager`
    //Serial.println("Building settings modal");

    okButton.setCallback([this]() { 
        // np. zapisz zmiany
        //Serial.println("OK button clicked in modal");
        hide(); 
        if(onClose) {
            onClose();
        }
    });

    closeButton.setCallback([this]() { 
        hide(); 
        if(onClose) {
            onClose();
        }
    });

    nextButton.setCallback([this]() { 
        SettingsManager::get().next(); 
        updateFromCurrentEntry();
    });
    prevButton.setCallback([this]() { 
        SettingsManager::get().previous(); 
        updateFromCurrentEntry();
    });
    plusButton.setCallback([this]() { 
        SettingsManager::get().increase(); 
        updateFromCurrentEntry();
    });
    minusButton.setCallback([this]() { 
        SettingsManager::get().decrease(); 
        updateFromCurrentEntry();
    });
    

    clickables.push_back(&okButton);
    clickables.push_back(&closeButton);
    clickables.push_back(&nextButton);
    clickables.push_back(&prevButton);
    clickables.push_back(&plusButton);
    clickables.push_back(&minusButton);
    uiElements.push_back(&valueLabel);
    uiElements.push_back(&entryNameLabel);
}

void Modal::updateFromCurrentEntry() {
        const auto& entry = SettingsManager::get().getCurrentEntry();
        if(entry.type == SettingEntry::FLOAT) {
            valueLabel.setText(String(*static_cast<float*>(entry.valuePtr), 4));
        } else if (entry.type == SettingEntry::ULONG) {
            valueLabel.setText(String(*static_cast<unsigned long*>(entry.valuePtr)));
        }
        
        entryNameLabel.setText(entry.name);
}