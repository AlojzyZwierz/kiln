#include "Modal.h"
Modal::Modal()
    : okButton("OK", 30, 180, 50, 30),
      closeButton("X", 270, 30, 25, 25),
      nextButton(">", 230, 110, 40, 40),
      prevButton("<", 50, 110, 40, 40),
      plusButton("+", 140, 60, 40, 40),
      minusButton("-", 140, 170, 40, 40),
      valueLabel("0", 110, 135, COLOR_BLACK, 2),
      entryNameLabel("XXX", 125, 165, COLOR_BLACK, 1),
      ipLabel("IP:", 180, 210, COLOR_BLACK, 1),
      cjTempLabel("CJ:", 190, 190, COLOR_BLACK, 1),
      cancelButton("Cancel", 140, 180, 65, 30),
    infoLabel1("...", 27, 62, COLOR_BLACK, 1),
    infoLabel2("...", 27, 80, COLOR_BLACK, 1),
    currentTempLabel("",280,65, COLOR_BLACK, 1 ),
    expectedTempLabel("",280,80, COLOR_BLACK, 1 )
    
{
    clickables.push_back(&okButton);
    clickables.push_back(&closeButton);
    clickables.push_back(&nextButton);
    clickables.push_back(&prevButton);
    clickables.push_back(&plusButton);
    clickables.push_back(&minusButton);
    clickables.push_back(&cancelButton);
    uiElements.push_back(&valueLabel);
    uiElements.push_back(&entryNameLabel);
    uiElements.push_back(&ipLabel);
    uiElements.push_back(&cjTempLabel);
    uiElements.push_back(&infoLabel1);
    uiElements.push_back(&infoLabel2);
    uiElements.push_back(&expectedTempLabel);
    uiElements.push_back(&currentTempLabel);
    // uiElements.push_back(&infoLabel);
}
void Modal::show(ModalMode mode, const String &extra, std::function<void()> confirmCallback)
{
    Serial.println("Modal::show() called with mode: " + String(static_cast<int>(mode)) + " extra: " + extra);

    visible = true;
    currentMode = mode;
    // uiElements.clear();
    // clickables.clear();
    infoMessage = "";
    for (auto *el : clickables)
    {
        el->setVisible(false);
    }
    for (auto *el : uiElements)
    {
        el->setVisible(false);
    }
    switch (mode)
    {
    case ModalMode::Settings:
        buildSettings();
        break;
    case ModalMode::Error:
        buildError(extra);
        break;
    case ModalMode::Confirmation:
        buildConfirmation(extra, confirmCallback);
        break;
    case ModalMode::Info:
        buildInfo();
        break;

    default:
        break;
    }
}
void Modal::hide()
{
    visible = false;
    Serial.println("Modal::hide() called");
    // uiElements.clear();
    // clickables.clear();
}

void Modal::render(TFT_eSprite &sprite)
{
    if (!visible)
        return;

    sprite.fillRect(20, 20, 280, 200, COLOR_MODAL_BG);
    sprite.drawRect(20, 20, 280, 200, COLOR_BLACK); // outline
    sprite.setTextDatum(MC_DATUM);
    sprite.drawString(title, 160, 40);
    infoLabel1.setText(String(ProcessController::get().getP(),3)+ " " + String(ProcessController::get().getI(),3) );
    infoLabel2.setText(String(ProcessController::get().getD(),3)+ " " + String(ProcessController::get().getRatio(),3) );
    cjTempLabel.setText(String( ProcessController::get().getCJTemp()));
    ipLabel.setText(WiFi.localIP().toString());
    expectedTempLabel.setText(String(ProcessController::get().getExpectedTemp()));
    currentTempLabel.setText(String(ProcessController::get().getCurrentTemp()));

    for (auto *el : uiElements)
    {
        if (el->isVisible())
            el->render(sprite);
    }

    for (auto *btn : clickables)
    {
        if (btn->isVisible())
            btn->render(sprite);
    }

    // np. info dialog
    if (currentMode == ModalMode::Info && !infoMessage.isEmpty())
    {
        sprite.setTextDatum(BL_DATUM);
        sprite.drawString(infoMessage, 30, 100);
    }
}

bool Modal::handleClick(int x, int y)
{
    if (!visible)
        return false;
    // Serial.println("Modal::handleClick() called at: " + String(clickables.size()) + " " + String(x) + " " + String(y));
    for (auto *el : clickables)
    {
        if (el->handleClick(x, y))
        {
            // Serial.println("Clickable element clicked: " + String(el->isVisible()) + " " +  String(x) + " " + String(y));
            return true; // kliknięcie obsłużone
        }
    }

    return false; // żaden przycisk nie został kliknięty
}

void Modal::buildSettings()
{
    title = "Settings";

    Serial.println("Building settings modal with " + String(cjTemp));
    updateFromCurrentEntry(); // Ustawia aktualną wartość i nazwę wpisu
    // Przyciski np. do zwiększania/zmniejszania wartości
    // lub np. delegacja do `SettingsManager`
    // Serial.println("Building settings modal");

    okButton.setCallback([this]()
                         { 
        // np. zapisz zmiany
        //Serial.println("OK button clicked in modal");
        StorageManager::saveSettings(); // Zapisz ustawienia
        hide(); 
        if(onClose) {
            onClose();
        } });

    closeButton.setCallback([this]()
                            { 
        hide(); 
        if(onClose) {
            onClose();
        } });

    nextButton.setCallback([this]()
                           { 
        SettingsManager::get().next(); 
        updateFromCurrentEntry(); });
    prevButton.setCallback([this]()
                           { 
        SettingsManager::get().previous(); 
        updateFromCurrentEntry(); });
    plusButton.setCallback([this]()
                           { 
        SettingsManager::get().increase(); 
        updateFromCurrentEntry(); });
    minusButton.setCallback([this]()
                            { 
        SettingsManager::get().decrease(); 
        updateFromCurrentEntry(); });

    okButton.setVisible(true);
    closeButton.setVisible(true);
    nextButton.setVisible(true);
    prevButton.setVisible(true);
    plusButton.setVisible(true);
    minusButton.setVisible(true);
    valueLabel.setVisible(true);
    entryNameLabel.setVisible(true);
    infoLabel1.setVisible(true);
    infoLabel2.setVisible(true);
     ipLabel.setVisible(true);
     cjTempLabel.setVisible(true);
     currentTempLabel.setVisible(true);
     expectedTempLabel.setVisible(true);
}

void Modal::buildError(const String &errorMessage)
{

    okButton.setCallback([this]()
                         {
        hide();
        if (onClose) {
            onClose();
        } });

    okButton.setVisible(true);
    title = "Error";
    infoMessage = errorMessage;
    entryNameLabel.setText(errorMessage);
    entryNameLabel.setVisible(true);
}

void Modal::updateFromCurrentEntry()
{
    const auto &entry = SettingsManager::get().getCurrentEntry();
    if (entry.type == SettingEntry::FLOAT)
    {
        valueLabel.setText(String(*static_cast<float *>(entry.valuePtr), 4));
    }
    else if (entry.type == SettingEntry::ULONG)
    {
        valueLabel.setText(String(*static_cast<unsigned long *>(entry.valuePtr)));
    }

    entryNameLabel.setText(entry.name);
}

void Modal::buildConfirmation(const String &message, std::function<void()> confirmCallback)
{
    title = "you sure?";
    infoMessage = message;
    entryNameLabel.setText(message);
    entryNameLabel.setVisible(true);

    okButton.setCallback([this]()
                         {
                             hide();
                             if (onConfirm)
                                 onConfirm(); // <-- WYKONAJ oryginalną akcję!
                         });

    cancelButton.setCallback([this]()
                             { hide();
                                if (onClose)                                     onClose(); });

    okButton.setVisible(true);
    cancelButton.setVisible(true);

    onConfirm = confirmCallback; // <-- ZAPAMIĘTAJ CO MIAŁO SIĘ STAĆ
}
void Modal::buildInfo()
{
    ipLabel.setVisible(true);
    cjTempLabel.setVisible(true);
    ipLabel.setText(WiFi.localIP().toString());    // Ustawia IP urządzenia
    cjTempLabel.setText(String(cjTemp, 2) + " C"); // Ustawia temperaturę cold junction
    /// infoLabel.setVisible(true);
    closeButton.setVisible(true);
    infoMessage = "P:" + String(ProcessController::get().getP()) + "\nI: " + String(ProcessController::get().getI()) + "\nD: " + String(ProcessController::get().getD()) + "\nr: " + String(ProcessController::get().getRatio());
    closeButton.setCallback([this]()
                            { 
        hide(); 
        if(onClose) {
            onClose();
        } });
}