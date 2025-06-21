#include "GUIRenderer.h"
#include "FakeFurnace.h"
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

                            startButton("Start", 150, 200, 60, 25, COLOR_BUTTON, COLOR_BLACK),
                            editButton("Edit", 220, 200, 60, 25, COLOR_BUTTON, COLOR_BLACK),
                            stopButton("Stop", 170, 200, 90, 25, COLOR_BUTTON, COLOR_BLACK),
                            graphRenderer(sprite, cm, temperatureSensor),
                            temperatureLabel("Temperature", 25, 40, COLOR_BLACK, 2),
                            curveIndexLabel("Curve Index", 25, 56, COLOR_BLACK, 1),
                            expectedTempLabel("Expected Temp", 25, 75, COLOR_BLACK, 1),
                            timeLabel("Time", 25, 70, COLOR_BLACK, 2),
                            segmentIndexLabel("Segment Index", 5, 40, COLOR_BLACK, 1), // Dodajemy etykietę dla segmentu
                            editCircle(110, 90, 70, cm),                               // Dodajemy okrąg edycyjny
                            closeButton("X", 290, 13, 17, 20, COLOR_BUTTON, COLOR_BLACK),
                            saveButton("Save", 265, 200, 50, 25, COLOR_BUTTON, COLOR_BLACK),
                            endHereButton("Cut", 220, 200, 40, 25, COLOR_BUTTON, COLOR_BLACK),
                            holdButton("Hold", 175, 200, 40, 25, COLOR_BUTTON, COLOR_BLACK),
                            skipButton("Skip", 130, 200, 40, 25, COLOR_BUTTON, COLOR_BLACK),
                            settingsButton("S", 290, 13, 17, 20, COLOR_BUTTON, COLOR_BLACK),
                            energyMeter(em),
                            costLabel("", 25, 90, COLOR_BLACK, 1),
                            errorLabel("X", 25, 220, COLOR_RED_DOT, 2),
                            infoButton("I", 270, 13, 17, 20, COLOR_BUTTON, COLOR_BLACK)

{
    SystemState::get().onModeChange = [this](SystemMode newMode)
    {
        this->setupUIFormodes(newMode);
        render(); // Odśwież GUI po zmianie trybu
    };
    ProcessController::get().setErrorCallback([this](const String &reason)
                                              { this->modal.show(ModalMode::Error, reason); });

    // Możesz też od razu ustawić początkowy stan GUI
    setupUIFormodes(SystemState::get().getMode());
    startButton.setCallback([&]()
                            {
                                ProcessController::get().startFiring();
                                // setMode(SystemMode::Firing);
                            });
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
    uiElements.push_back(&temperatureLabel);
    uiElements.push_back(&curveIndexLabel);
    uiElements.push_back(&expectedTempLabel);
    uiElements.push_back(&timeLabel);
    uiElements.push_back(&segmentIndexLabel);
    uiElements.push_back(&costLabel);
    uiElements.push_back(&errorLabel);

    //  Serial.println("Clickables in list in constructor: " + String(clickables.size()));
    sprite.setFreeFont(FONT_SMALL);

    modal.onClose = [this]()
    {
        this->render(); // albo render(), co tam masz
    };
}

void GUIRenderer::render()
{
    // Serial.println("TFT_YELLOW: " + String(TFT_YELLOW)+ " uiPalette[COLOR_BUTTON]: " + String(uiPalette[COLOR_BUTTON]) + " uiPalette[COLOR_MODAL_BG]: " + String(uiPalette[COLOR_MODAL_BG]) + " uiPalette[COLOR_COOLING_LINE]: " + String(uiPalette[COLOR_COOLING_LINE]));

    sprite.deleteSprite(); // Usuwamy poprzedni sprite
    sprite.setColorDepth(8);
    sprite.createSprite(TFT_HEIGHT, TFT_WIDTH);
    // sprite.createPalette(255);
    // Serial.printf("Głębia kolorów sprite'a: %d\n", sprite.getColorDepth());

    // Serial.println("kolor" + String(COLOR_BG) + " " + String( uiPalette[COLOR_BUTTON]) + " " + String( uiPalette[COLOR_MODAL_BG]) + " " + String( uiPalette[COLOR_COOLING_LINE]));
    if (!sprite.created())
    {
        Serial.println("Sprite creation failed!");
    }
    else
    {
        // Serial.println("Sprite created successfully!");
    }
    sprite.fillSprite(COLOR_BG);

    graphRenderer.render(); // Rysuje wykres
    if (SystemState::get().getMode() == SystemMode::Firing)
    {
        sprite.drawRect(0, 0, TFT_HEIGHT, TFT_WIDTH, COLOR_RED_DOT); // Rysujemy czerwoną ramkę
        sprite.drawRect(1, 1, TFT_HEIGHT - 2, TFT_WIDTH - 2, COLOR_RED_DOT);
        sprite.drawRect(2, 2, TFT_HEIGHT - 4, TFT_WIDTH - 4, COLOR_RED_DOT);
    }
    drawHeader();
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
        modal.setCJTemp(temperatureSensor.getCJTemperature()); // Ustawiamy temperaturę cold junction w modalu
        modal.setTCVoltage(temperatureSensor.getRawVoltage());
        modal.setTCTemp(temperatureSensor.readThermocoupleTemperature());
        //  Serial.println("Rendering modal");
        modal.render(sprite); // Rysujemy modal, jeśli jest widoczny
    }
    // uint16_t test_color = sprite.getPaletteColor(1); // Powinien zwrócić uiPalette[1]

    sprite.pushSprite(0, 0); // Wyświetlamy sprite na ekranie
}

void GUIRenderer::drawHeader()
{
    // float temperature = tempSensor.getTemperature();

    float temperature = SystemState::get().getMode() != SystemMode::Edit ? temperatureSensor.getTemperature() : curveManager.getOriginalCurve().elems[curveManager.getSegmentIndex()].endTemp; // potrzebujesz takiej metody
    int curveIndex = curveSelector.getSelectedIndex();                                                                                                                                         // potrzebujesz takiej metody

    temperatureLabel.setText(String(temperature, 1) + "'C");
    String segInd = (SystemState::get().getMode() == SystemMode::Idle) ? " " : ("." + String(curveManager.getSegmentIndex() + 1));
    // Serial.println(curveManager.getSegmentIndex());
    curveIndexLabel.setText("prog #" + String(curveIndex) + segInd);
    expectedTempLabel.setText("e:" + String(ProcessController::get().getExpectedTemp(), 1) + "'C"); // potrzebujesz takiej metody
    // timeLabel.setText("Time: " + String(curveManager.getTotalTime()) + "s"); // potrzebujesz takiej metody
    segmentIndexLabel.setText(String(curveManager.getSegmentIndex() + 1));                                                                                                                                                 // potrzebujesz takiej metody,
    timeLabel.setText((curveManager.isSkip()) ? "skip" : Utils::millisToHM(curveManager.getOriginalCurve().elems[curveManager.getSegmentIndex()].hTime)); // potrzebujesz takiej metody
    float cost = energyMeter.getCost();                                                                                                                                                                                    // potrzebujesz takiej metody
    if (cost > 0.01f)
        costLabel.setText(String(cost, 2) + " zl"); // potrzebujesz takiej metody
    else
        costLabel.setText(" ");
    int errorCount = temperatureSensor.getErrorCount();
    if (errorCount != 0)
    {
        errorLabel.setText("Error:" + String(temperatureSensor.GetLastErrorCode()) + " : " + String(errorCount));
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
    // Serial.println("Clickables on list: " + String(clickables.size()));
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
        startButton.setVisible(true);
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
        settingsButton.setCallback([&]()
                                   { modal.show(ModalMode::Settings); });
        infoButton.setCallback([&]()
                               { modal.show(ModalMode::Info); });
        break;
    case SystemMode::Edit:
        curveManager.setSegmentIndex(0);
        leftArrow.setVisible(true);
        rightArrow.setVisible(true);
        timeLabel.setVisible(true);
        segmentIndexLabel.setVisible(true);
        editCircle.setVisible(true);
        segmentIndexLabel.setVisible(true);
        temperatureLabel.setVisible(true);
        closeButton.setVisible(true);
        saveButton.setVisible(true);
        endHereButton.setVisible(true);
        holdButton.setVisible(true);
        skipButton.setVisible(true);

        saveButton.setCallback([&]()
                               {
                if(curveManager.getSegmentIndex() < curveElemsNo-1) {
                 //   curveManager.getOriginalCurve().elems[0].hTime = 0;
                }
                StorageManager::saveCurve(curveManager, curveSelector.getSelectedIndex());
                Serial.println(curveManager.getOriginalCurve().toString());
                SystemState::get().setMode(SystemMode::Idle); });
        /*            saveButton.setCallback([&]() {
            StorageManager::saveCurve(curveManager, curveSelector.getSelectedIndex());
            setMode(SystemMode::Idle);
        });*/
        leftArrow.setCallback([&]()
                              {
              if (curveManager.getSegmentIndex()>0 )curveManager.setSegmentIndex(curveManager.getSegmentIndex() - 1); });
        rightArrow.setCallback([&]()
                               {
                int segIndex = curveManager.getSegmentIndex();
                if( segIndex < curveElemsNo-2){
                    curveManager.setSegmentIndex(segIndex + 1);
                    if(curveManager.getOriginalCurve().elems[segIndex + 1].hTime == 0) {
                        //Curve  curve = curveManager.getOriginalCurve();
                        curveManager.updateTime(segIndex + 1, 3600000);
                        curveManager.updateTemperature(segIndex + 1, curveManager.getOriginalCurve().elems[segIndex].endTemp);
                        curveManager.updateTime(segIndex + 2, 0);
                        /*curve.elems[segIndex +2].hTime = 0;
                            curve.elems[segIndex +1].hTime = 3600000;
                            curve.elems[segIndex +1].endTemp = curve.elems[segIndex].endTemp;
                            curveManager.loadOriginalCurve(curve);*/
                           // Serial.println("+++ " + String(curve.elems[segIndex +1].hTime) + " " + String(curve.elems[segIndex +1].endTemp) );
                           // Serial.println("+++++ " + String(curve.elems[segIndex +2].hTime) + " " + String(curve.elems[segIndex +2].endTemp) );
                        //Serial.println(String(curve.toString()));
                       // Serial.println("_____"+String(curveManager.getOriginalCurve().toString()));
                       // Serial.println("___**__"+String(curveManager.getAdjustedCurve().toString()));
                }    
            } });
        endHereButton.setCallback([&]()
                                  {
                                      if (curveManager.getSegmentIndex() + 1 < curveElemsNo)
                                          curveManager.updateTime(curveManager.getSegmentIndex() + 1, 0);
                                      //                    Serial.println("End here pressed " + curveManager.getOriginalCurve().toString());
                                  });

        holdButton.setCallback([&]()
                               {
                                      if(curveManager.getSegmentIndex() >0){
                                        curveManager.updateTemperature(curveManager.getSegmentIndex(), curveManager.getOriginalCurve().elems[curveManager.getSegmentIndex() - 1].endTemp);
                                      } });
        skipButton.setCallback([&]()
                               {
            if (curveManager.getSegmentIndex() + 1 < curveElemsNo)
            {
                curveManager.updateTime(curveManager.getSegmentIndex() , 60000);
            } });
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

        //     ProcessController::get().abort("Aborted by user");
        //       SystemState::get().setMode(SystemMode::Idle); });
        settingsButton.setVisible(true);
        settingsButton.setCallback([&]()
                                   { modal.show(ModalMode::Settings); });

        break;
    }
}
