#include "GUIRenderer.h"
//  #include "FakeFurnace.h"
// extern FakeFurnace furnace;
GUIRenderer::GUIRenderer(
    TFT_eSPI &display,
    TemperatureSensor &sensor,
    CurveSelector &cs,
    CurveManager &cm,
    TemperatureSensor &ts,
    EnergyUsageMeter &em) : tft(display),
                            sprite(&display),
                            temperatureSensor(sensor),
                            curveSelector(cs),
                            curveManager(cm),
                            leftArrow(10, 110, 30, ArrowDirection::Left, COLOR_BLACK),
                            rightArrow(280, 110, 30, ArrowDirection::Right, COLOR_BLACK),
                            startButton("start", 150, 200, 60, 25, COLOR_BUTTON, COLOR_BLACK),
                            editButton("edit", 220, 200, 60, 25, COLOR_BUTTON, COLOR_BLACK),
                            stopButton("stop", 170, 200, 90, 25, COLOR_BUTTON, COLOR_BLACK),
                            closeButton("X", 290, 13, 17, 20, COLOR_BUTTON, COLOR_BLACK),
                            insertButton("insert", 156, 192, 52, 21, COLOR_BUTTON, COLOR_BLACK),
                            skipButton("skip", 211, 192, 52, 21, COLOR_BUTTON, COLOR_BLACK),
                            holdButton("hold", 266, 192, 52, 21, COLOR_BUTTON, COLOR_BLACK),
                            endHereButton("cut", 156, 215, 52, 21, COLOR_BUTTON, COLOR_BLACK),
                            saveButton("save", 211, 215, 107, 21, COLOR_BUTTON, COLOR_BLACK),
                            settingsButton("S", 290, 13, 17, 20, COLOR_BUTTON, COLOR_BLACK),
                            infoButton("I", 270, 13, 17, 20, COLOR_BUTTON, COLOR_BLACK),
                            armedButton("armed", 150, 200, 60, 25, COLOR_BUTTON, COLOR_BLACK, 1),
                            graphRenderer(sprite, cm, temperatureSensor),
                            editCircle(160, 90, 70, cm),
                            energyMeter(em),
                            temperatureLabel("Temperature", 15, 36, COLOR_BLACK, 2),
                            curveIndexLabel("Curve Index", 16, 52, COLOR_BLACK, 1),
                            expectedTempLabel("Expected Temp", 16, 71, COLOR_BLACK, 1),
                            costLabel("", 16, 86, COLOR_BLACK, 1),
                            segmentIndexLabel("Segment Index", 5, 40, COLOR_BLACK, 1),
                            errorLabel("X", 25, 220, COLOR_RED_DOT, 2),
                            infoBox(2, 2, COLOR_BLACK, 1, 1, 1, -4)

{
    SystemState::get().onModeChange = [this](SystemMode newMode)
    {
        this->setupUIFormodes(newMode);
        render(); // Odśwież GUI po zmianie trybu
    };

    SystemState::get().onArmedChange = [this](bool)
    {
        if (SystemState::get().getMode() == SystemMode::Idle)
            this->setupUIFormodes(SystemMode::Idle);
        render();
    };
    ProcessController::get().setErrorCallback([this](const String &reason)
                                              { this->modal.show(ModalMode::Error, reason); });

    ProcessController::get().setMessageCallback([this](const String &message)
                                                { this->modal.show(ModalMode::Message, message); });
    // Możesz też od razu ustawić początkowy stan GUI
    setupUIFormodes(SystemState::get().getMode());
    startButton.setCallback([&]()
                            { modal.show(ModalMode::DelayedStart); });
    editButton.setCallback([&]()
                           {
            //curveSelector.selectNext();
            SystemState::get().setMode(SystemMode::Edit); });
    closeButton.setCallback([&]()
                            {
            SystemState::get().setMode(SystemMode::Idle);
            StorageManager::loadCurve(curveManager, curveSelector.getSelectedIndex()); });
    infoButton.setCallback([&]()
                           { modal.show(ModalMode::Info); });

    //armedButton.setCallback([&]()
            //                { SystemState::get().setArmed(false, 0, 0); });
    armedButton.setCallback([&]()
                            { modal.show(ModalMode::Confirmation, "Cancel delayed start?", []()
                                         { SystemState::get().setArmed(false, 0, 0); }); });
    clickables.push_back(&startButton);
    clickables.push_back(&leftArrow);
    clickables.push_back(&rightArrow);
    clickables.push_back(&editButton);
    clickables.push_back(&stopButton);
    clickables.push_back(&closeButton);
    clickables.push_back(&saveButton);
    clickables.push_back(&endHereButton);
    clickables.push_back(&editCircle);
    clickables.push_back(&settingsButton);
    clickables.push_back(&holdButton);
    clickables.push_back(&skipButton);
    clickables.push_back(&infoButton);
    clickables.push_back(&insertButton);
    clickables.push_back(&armedButton);
    uiElements.push_back(&temperatureLabel);
    uiElements.push_back(&curveIndexLabel);
    uiElements.push_back(&expectedTempLabel);
    // uiElements.push_back(&timeLabel);
    uiElements.push_back(&segmentIndexLabel);
    uiElements.push_back(&costLabel);
    uiElements.push_back(&errorLabel);
    uiElements.push_back(&infoBox);

    //  Serial.println("Clickables in list in constructor: " + String(clickables.size()));
    sprite.setFreeFont(FONT_SMALL);

    modal.onClose = [this]()
    {
        this->render(); // albo render(), co tam masz
    };

    modal.onDelayConfirmed = [this](int h)
    {
        if (h == 0)
            ProcessController::get().startFiring();
        else
            SystemState::get().setArmed(true, (uint32_t)h,
                                        curveSelector.getSelectedIndex());
    };
}

void GUIRenderer::render()
{
    // Serial.println("TFT_YELLOW: " + String(TFT_YELLOW)+ " uiPalette[COLOR_BUTTON]: " + String(uiPalette[COLOR_BUTTON]) + " uiPalette[COLOR_MODAL_BG]: " + String(uiPalette[COLOR_MODAL_BG]) + " uiPalette[COLOR_COOLING_LINE]: " + String(uiPalette[COLOR_COOLING_LINE]));

    if (!sprite.created())
    {
        initSprite(); // tylko za pierwszym razem
        if (!sprite.created())
            return; // zamiast rysować po nullptr
    }
    // sprite.createPalette(255);
    // Serial.printf("Głębia kolorów sprite'a: %d\n", sprite.getColorDepth());

    // Serial.println("kolor" + String(COLOR_BG) + " " + String( uiPalette[COLOR_BUTTON]) + " " + String( uiPalette[COLOR_MODAL_BG]) + " " + String( uiPalette[COLOR_COOLING_LINE]));

    sprite.fillSprite(COLOR_BG);

    graphRenderer.render(); // Rysuje wykres
    if (SystemState::get().getMode() == SystemMode::Firing)
        drawFrame(COLOR_RED);
    else if (SystemState::get().isArmed())
        drawFrame(COLOR_COOLING_LINE);

    if (SystemState::get().getMode() == SystemMode::Edit)
    {
        infoBox.setLine(0, "%.0f C", curveManager.getSegmentTemp());
        int h = curveManager.getSegmentTime() / 3600000;
        int m = (curveManager.getSegmentTime() % 3600000) / 60000;
        if (curveManager.isSkip())
        {
            infoBox.setLine(1, "skip");
            infoBox.setLine(2, "");
        }
        else
        {
            infoBox.setLine(1, "%d:%02d", h, m);
            infoBox.setLine(2, "%+.0f C/h", curveManager.getHeatingSpeed());
        }
    }
    drawHeader();
    
    armedButton.setText("A" + Utils::millisToHM(SystemState::get().getArmedRemainingMs() + 59999)); // +500ms dla zaokrąglenia do góry
    for (auto &uIElement : uiElements)
    { // Używamy referencji (auto&), aby uniknąć kopiowania
        if (uIElement->isVisible())
        {                              // Opcjonalne sprawdzenie widoczności
            uIElement->render(sprite); // Wywołanie wirtualnej metody render()
        }
    }
    for (auto &uIElement : clickables)
    { // Używamy referencji (auto&), aby uniknąć kopiowania
        if (uIElement->isVisible())
        {                              // Opcjonalne sprawdzenie widoczności
            uIElement->render(sprite); // Wywołanie wirtualnej metody render()
        }
    }
    // sprite.drawCircle(110, 90, 70, TFT_YELLOW); // Rysujemy okrąg
    // Serial.println("modal visible: " + String(modal.isVisible()));

    if (modal.isVisible())
{
    if (modal.needsSensorData() && millis() - lastSensorRead >= 1000)
    {
        lastSensorRead = millis();
        modal.setCJTemp(temperatureSensor.getCJTemperature());
        modal.setTCTemp(temperatureSensor.readThermocoupleTemperature());
    }
    modal.render(sprite);
}
    // uint16_t test_color = sprite.getPaletteColor(1); // Powinien zwrócić uiPalette[1]

    sprite.pushSprite(0, 0); // Wyświetlamy sprite na ekranie
}

void GUIRenderer::initSprite()
{
    sprite.setColorDepth(4);                    // PRZED createSprite
    sprite.createSprite(TFT_HEIGHT, TFT_WIDTH); // 320x240 = 38 400 B
    if (!sprite.created())
    {
        Serial.println("Sprite creation failed!");
        return;
    }
    buildCustomPalette();
    sprite.createPalette(uiPalette, UI_PALETTE_SIZE); // PO createSprite
}

void GUIRenderer::drawHeader()
{
    // float temperature = tempSensor.getTemperature();

    float temperature = (temperatureSensor.getTemperature() + 0.5f);
    int curveIndex = curveSelector.getSelectedIndex();

    temperatureLabel.setText(String((int)temperature));
    // Serial.println(curveManager.getSegmentIndex());
    curveIndexLabel.setText(
        "prog #" + String(curveIndex) +
        ((SystemState::get().getMode() != SystemMode::Firing)
             ? String(" ")
             : ("/" + String(curveManager.getSegmentIndex() + 1))));
    expectedTempLabel.setText((!curveManager.isSkip() ? "e:" + String((int)(ProcessController::get().getExpectedTemp() + 0.5f)) : " "));
    // timeLabel.setText("Time: " + String(curveManager.getTotalTime()) + "s"); // potrzebujesz takiej metody
    // segmentIndexLabel.setText(String(curveManager.getSegmentIndex() + 1));
    // timeLabel.setText((curveManager.isSkip()) ? "skip" : (Utils::millisToHM(curveManager.getOriginalCurve().elems[curveManager.getSegmentIndex()].hTime) + " (" + String((int)curveManager.getHeatingSpeed()) + ")")); // potrzebujesz takiej metody
    float cost = energyMeter.getCost(); // potrzebujesz takiej metody
    if (cost > 0.01f)
        costLabel.setText(String(cost, 2) + " zl"); // potrzebujesz takiej metody
    else
        costLabel.setText(" ");
    int errorCount = temperatureSensor.getErrorCount();
    if (errorCount != 0)
    {
        sprite.setTextSize(1);
        errorLabel.setText("Err:" + String(temperatureSensor.GetLastErrorCode()) + " - " + String(errorCount));
    }
    else
        errorLabel.setText(" ");
    /*  sprite.setFreeFont(FONT_LARGE);
      sprite.setTextSize(1.8);

      sprite.setCursor(25, 40);
      sprite.printf("%.1f°C", temperature);

      // Numer krzywej - mniejsza czcionka
      sprite.setFreeFont(FONT_MEDIUM);
      sprite.setTextSize(1);
      sprite.setCursor(25, 60);
      sprite.print("e: " +  String( ProcessController::get().getExpectedTemp())); // potrzebujesz takiej metody

      sprite.setCursor(25, 75);
      sprite.printf("Prog #%d", curveIndex);
      for(auto& uiElement : uiElements) {
          if (uiElement->isVisible()) {
              uiElement->render(sprite); // Rysujemy elementy UI
          }
      }
          */
}

void GUIRenderer::handleTouch(int x, int y)
{
    // Serial.println("Handling touch at: " + String(x) + ", " + String(y));
    //  Serial.println("Clickables on list: " + String(clickables.size()));
    if (modal.isVisible())
    {

        if (modal.handleClick(x, y))
        {
            // Serial.println("Modal clicked at: " + String(x) + ", " + String(y));
            return; // Kliknięcie obsłużone przez modal
        }
    }
    for (const auto &clickable : clickables)
    {
        // Serial.println("visible-" + String(clickable->isVisible()) + " active-" + String(clickable->isActive()));
        if (clickable->isVisible())
        { // dodać sprawdzenie aktywności
          // Serial.println("Checking clickable at: " + String(x) + ", " + String(y));
            if (clickable->handleClick(x, y))
                break;
        }
    }
}

void GUIRenderer::setupUIFormodes(SystemMode mode)
{
    for (auto *c : uiElements)
    {
        c->setVisible(false);
        // c->setVisible(false); // jeśli masz widoczność
    }
    for (auto *c : clickables)
    {
        c->setVisible(false);
        // c->setVisible(false); // jeśli masz widoczność
    }
    switch (mode)
    {
    case SystemMode::Idle:
        // infoButton.setVisible(true);
        errorLabel.setVisible(true);
        // bool armed = SystemState::get().isArmed();
        startButton.setVisible(!SystemState::get().isArmed());
        armedButton.setVisible(SystemState::get().isArmed());
        // armedButton.setText("armed " + Utils::millisToHM(SystemState::get().getArmedRemainingMs()));

        editButton.setVisible(true);
        leftArrow.setVisible(true);
        rightArrow.setVisible(true);
        leftArrow.setCallback([&]()
                              {
                                  curveSelector.selectPrevious();
                                  // Serial.println("Selected previous curve index: " + String(curveSelector.getSelectedIndex()));
                              });
        rightArrow.setCallback([&]()
                               {
                                   curveSelector.selectNext(); // musisz mieć taką metodę!
                               });
        temperatureLabel.setVisible(true);
        curveIndexLabel.setVisible(true);
        // curveManager.setSegmentIndex(-1); // Resetowanie indeksu segmentu
        settingsButton.setVisible(true);
        costLabel.setVisible(true);
        /*modal.onConfirm = [this]()
        {
            SystemState::get().setDelayedCurveIndex(curveManager.getcurrentCurveIndex());
        };*/
        settingsButton.setCallback([&]()
                                   { modal.show(ModalMode::Settings); });
        infoButton.setCallback([&]()
                               { modal.show(ModalMode::Info); });

        break;
    case SystemMode::Edit:
        curveManager.setSegmentIndex(0);
        leftArrow.setVisible(true);
        rightArrow.setVisible(true);
        // timeLabel.setVisible(true);
        editCircle.setVisible(true);
        closeButton.setVisible(true);
        saveButton.setText(SystemState::get().isArmed() &&
                                   SystemState::get().getDelayedCurveIndex() == curveSelector.getSelectedIndex()
                               ? "save armed"
                               : "save");
        saveButton.setVisible(true);
        endHereButton.setVisible(true);
        holdButton.setVisible(true);
        skipButton.setVisible(true);
        insertButton.setVisible(true);
        infoBox.setVisible(true);

        saveButton.setCallback([&]()
                               {
                StorageManager::saveCurve(curveManager, curveSelector.getSelectedIndex());
                Serial.println(curveManager.getOriginalCurve().toString());
                SystemState::get().setMode(SystemMode::Idle); });
        /*            saveButton.setCallback([&]() {
            StorageManager::saveCurve(curveManager, curveSelector.getSelectedIndex());
            setMode(SystemMode::Idle);
        });*/
        leftArrow.setCallback([&]()
                              { curveManager.prevSegment(); });
        rightArrow.setCallback([&]()
                               {
                                        if (curveManager.hasNextSegment())          curveManager.nextSegment();
                                        else if (curveManager.canAddSegment())      curveManager.addSegment(); });
        endHereButton.setCallback([&]()
                                  { curveManager.endHere(); });
        holdButton.setCallback([&]()
                               { curveManager.setHold(); });
        skipButton.setCallback([&]()
                               { curveManager.setSkipAuto(); });
        insertButton.setCallback([&]()
                                 { curveManager.insertSegment(curveManager.getSegmentIndex()); });
        break;
    case SystemMode::Firing:
        // infoButton.setVisible(true);
        errorLabel.setVisible(true);
        costLabel.setVisible(true);
        stopButton.setVisible(true);
        temperatureLabel.setVisible(true);
        curveIndexLabel.setVisible(true);
        expectedTempLabel.setVisible(true);
        stopButton.setCallback([&]()
                               { modal.show(ModalMode::Confirmation, "Stop firing?", []()
                                            { ProcessController::get().abort("Aborted by user"); }); });
        settingsButton.setVisible(true);
        settingsButton.setCallback([&]()
                                   { modal.show(ModalMode::Settings); });

        break;
    }
}

void GUIRenderer::drawFrame(uint8_t color)
{
    for (int i = 0; i < 3; i++)
        sprite.drawRect(i, i, TFT_HEIGHT - 2 * i, TFT_WIDTH - 2 * i, color);
}
