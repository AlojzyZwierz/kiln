#include <Adafruit_BusIO_Register.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <Arduino.h>
#include <esp_heap_caps.h>

#include "GraphRenderer.h"
#include "CurveManager.h"
#include "StorageManager.h"
#include "TemperatureSensor.h"
#include "ProcessController.h"
#include "ResumeManager.h"
#include "HeatingController.h"
#include "GUIRenderer.h"
#include "FakeFurnace.h"
#include "ColorPalette.h"
#include "EnergyUsageMeter.h"
#include "WebServerManager.h"

TFT_eSPI tft = TFT_eSPI();

// Touchscreen pins
#define XPT2046_IRQ 36  // T_IRQ
#define XPT2046_MOSI 32 // T_DIN
#define XPT2046_MISO 39 // T_OUT
#define XPT2046_CLK 25  // T_CLK
#define XPT2046_CS 33   // T_CS
// #define BUZZERPIN 22
#define FONT_SIZE 2
// #define SSR 22 // pin przekaźnika – ustaw wg własnych potrzeb

SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);
CurveManager curveManager;
TemperatureSensor temperatureSensor;
CurveSelector curveSelector(curveManager);
// FakeFurnace furnace;
EnergyUsageMeter energyMeter;
HeatingController heatingController(energyMeter);
GUIRenderer guiRenderer(tft, temperatureSensor, curveSelector, curveManager, temperatureSensor, energyMeter);
WebServerManager webServerManager(curveManager, temperatureSensor, tft);

// ProcessController& controller = ProcessController::get();
// MeasurementManager measurementManager = MeasurementManager::get();

unsigned long lastUpdateTime = 0;
bool wasTouched = false;
unsigned long lastTouchTime = 0;
// MeasurementManager measurementManager;
unsigned long nextMeasurementTime = 0;
//unsigned long measurementInterval = 150000;//

void setup()
{
#ifdef NO_THERMOCOUPLE
  pinMode(BACKLIGHT_PIN, OUTPUT);
  digitalWrite(BACKLIGHT_PIN, LOW); // Włącz podświetlenie
#endif
  pinMode(SSR_PIN, OUTPUT);
  pinMode(BUZZERPIN, OUTPUT);
  Serial.begin(115200);
  delay(1000);
  while (StorageManager::begin() == false)
  {
    Serial.println("SPIFFS init failed. Retrying...");
    delay(1000);
  }
  curveSelector.selectByIndex(1);
  curveSelector.selectNext();
  ProcessController::get().begin(curveManager, temperatureSensor, heatingController, energyMeter);
  // StorageManager::saveSettings();
  StorageManager::loadSettings();

  // Serial.println("Settings loaded: " + String(SettingsManager::get().getSettings().pid_kp) + ", " + String(SettingsManager::get().getSettings().pid_ki) + ", " + String(SettingsManager::get().getSettings().pid_kd));
  // Serial.println("Settings loaded: " + String(SettingsManager::get().getSettings().heatingCycleMs) + ", " + String(SettingsManager::get().getSettings().kilnPower) + ", " + String(SettingsManager::get().getSettings().unitCost));
  temperatureSensor.begin();
  Serial.println("Temperature sensor initialized.");
  tft.init();
  Serial.println("TFT initialized.");
  // buildCustomPalette();
  //  Serial.println("Custom palette built.");
  tft.setRotation(1);
  tft.fillScreen(COLOR_BG);
  tft.setTextColor(COLOR_BLACK);
  tft.setTextSize(1);
  tft.setCursor(3, 200);
  tft.print(BUILD_TIME);
  tft.setTextSize(2);
  tft.setCursor(16, 50);
  tft.print("Initializing.");
  temperatureSensor.begin();
  Serial.println("tft done...");

  // controller.begin(curveManager, temperatureSensor);
  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touchscreen.begin(touchscreenSPI);
  Serial.println("Touchscreen initialized.");
  tft.print(".");
  // Set the Touchscreen rotation in landscape mode
  // Note: in some displays, the touchscreen might be upside down, so you might need to set the rotation to 3: touchscreen.setRotation(3);
  touchscreen.setRotation(3);
  // furnace.begin();
  webServerManager.begin();
  tft.print(".");
  SystemState::get().setMode(SystemMode::Idle);
  if (ResumeManager::hasResumeData())
  {
    int cIndex = ResumeManager::loadCurveIndex();
    Serial.print("resume ind: ");
    Serial.println(cIndex);
    curveSelector.selectByIndex(cIndex);
    ProcessController::get().startFiring();
  }
  tft.print(".");

  guiRenderer.render();
  SoundManager::chiptuneIntro();
  Utils::printMemoryInfo();
}

TS_Point lastP;
void loop()
{
  if (millis() > nextMeasurementTime && (SystemState::get().getMode() == SystemMode::Firing || SystemState::get().isCooling()))
  {
    nextMeasurementTime = millis() + MeasurementManager::get().getMeasurementInterval();
    MeasurementManager::get().addMeasurement();
  }
  if (SystemState::get().getMode() == SystemMode::Firing)
  {
    ProcessController::get().applyPID();
    heatingController.update();
    // furnace.update();
  }
  if (lastUpdateTime + 1001 < millis())
  {
    lastUpdateTime = millis();
    temperatureSensor.update();
    guiRenderer.render();
    if (SystemState::get().isCooling() && temperatureSensor.getTemperature() < 100)
    {
      SystemState::get().setCooling(false);
    }
    // Serial.println("GUI rendered at: " + String(millis()));
    // Serial.println("seg htime: " + String(curveManager.getOriginalCurve().elems[0].hTime) + " current segment index: " + String(curveManager.getSegmentIndex()) + " current temp: " + String(furnace.getTemperature()));
    if (SystemState::get().getMode() == SystemMode::Firing)
    {
      ProcessController::get().checkSegmentAdvance();
      ProcessController::get().checkForErrors();
      // ProcessController::get().adjustSkipTime();
    }
    // printMemoryInfo();
  }
  bool isTouched = touchscreen.tirqTouched() && touchscreen.touched();
  if (isTouched)
  {
    wasTouched = true;
    delay(200);
  }
  else
  {
    if (wasTouched)
    {

      TS_Point p = touchscreen.getPoint();
      int y = map(p.x, 400, 3800, 0, 240);
      int x = map(p.y, 3800, 300, 0, 320);
      // Serial.println("Klik " + String(x) + " " + String(y));

      guiRenderer.handleTouch(x, y);
      guiRenderer.render();
    }

    wasTouched = isTouched;
  }
}
