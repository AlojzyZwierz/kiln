#include <Adafruit_BusIO_Register.h>
#include <SPI.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <Arduino.h>

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
#define XPT2046_IRQ 36   // T_IRQ
#define XPT2046_MOSI 32  // T_DIN
#define XPT2046_MISO 39  // T_OUT
#define XPT2046_CLK 25   // T_CLK
#define XPT2046_CS 33    // T_CS

SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);
//#define BUZZERPIN 21
#define FONT_SIZE 2
//#define SSR 9 // pin przekaźnika – ustaw wg własnych potrzeb
CurveManager curveManager;
TemperatureSensor temperatureSensor;
CurveSelector curveSelector(curveManager);
FakeFurnace furnace;
EnergyUsageMeter energyMeter;
HeatingController heatingController(energyMeter);
GUIRenderer guiRenderer(tft, temperatureSensor,curveSelector , curveManager, furnace, energyMeter);
WebServerManager webServerManager(curveManager, furnace);

//ProcessController& controller = ProcessController::get();
//MeasurementManager measurementManager = MeasurementManager::get();
  
  unsigned long lastUpdateTime = 0;
bool wasTouched = false;
unsigned long lastTouchTime = 0;
//MeasurementManager measurementManager;
unsigned long nextMeasurementTime = 0;
unsigned long measurementInterval = 30000; 
void setup() {
  //pinMode(BUZZERPIN, OUTPUT);
  Serial.begin(115200);
  delay(1000);
  while(StorageManager::begin()  == false) {
    Serial.println("SPIFFS init failed. Retrying...");
    delay(1000);
  }
  ProcessController::get().begin(curveManager, temperatureSensor, heatingController);
  //StorageManager::saveSettings();
  StorageManager::loadSettings();
  //Serial.println("Settings loaded: " + String(SettingsManager::get().getSettings().pid_kp) + ", " + String(SettingsManager::get().getSettings().pid_ki) + ", " + String(SettingsManager::get().getSettings().pid_kd));
  //Serial.println("Settings loaded: " + String(SettingsManager::get().getSettings().heatingCycleMs) + ", " + String(SettingsManager::get().getSettings().kilnPower) + ", " + String(SettingsManager::get().getSettings().unitCost));
 
  tft.init();
  buildCustomPalette();
  
  tft.setRotation(1);
  tft.fillScreen(COLOR_BG);
  tft.setTextColor(COLOR_BLACK);
  
  curveSelector.selectByIndex(10);
  guiRenderer.render();
  //controller.begin(curveManager, temperatureSensor);
  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touchscreen.begin(touchscreenSPI);
  // Set the Touchscreen rotation in landscape mode
  // Note: in some displays, the touchscreen might be upside down, so you might need to set the rotation to 3: touchscreen.setRotation(3);
  touchscreen.setRotation(3);
  furnace.begin();
  webServerManager.begin();
  if(ResumeManager::hasResumeData()) {
    curveManager.setcurrentCurveIndex(ResumeManager::loadCurveIndex());
    ProcessController::get().startFiring();
    //SystemState::get().setMode(SystemMode::Firing);
  }
  SystemState::get().setMode(SystemMode::Idle);
}
TS_Point lastP;
void loop() {
  
  if(ProcessController::get().isRunning()) {  
    if(millis() > nextMeasurementTime) {
   // Serial.println("Adding measurement at: " + String(millis()) + " furnace temp: " + String(furnace.getTemperature()));
    nextMeasurementTime = millis() + measurementInterval;
    MeasurementManager::get().addMeasurement(millis() - ProcessController::get().getProgramStartTime(),furnace.getTemperature());
   // Serial.println("Measurements no: " + String(MeasurementManager::get().getMeasurements().size()));
  }
    if(lastUpdateTime + 1000 < millis()) {
      //Serial.println("Rendering GUI at: " + String(millis()));
      lastUpdateTime = millis();
      guiRenderer.render();
     // Serial.println("GUI rendered at: " + String(millis()));
    // Serial.println("seg htime: " + String(curveManager.getOriginalCurve().elems[0].hTime) + " current segment index: " + String(curveManager.getSegmentIndex()) + " current temp: " + String(furnace.getTemperature()));
      ProcessController::get().checkSegmentAdvance();
  }  
  ProcessController::get().applyPID();  
  
  furnace.update();    
  }
  bool isTouched = touchscreen.tirqTouched() && touchscreen.touched();
 if (isTouched) {
  wasTouched = true;
  delay(200);
 }else{
  if(wasTouched){
    
    TS_Point p = touchscreen.getPoint();
    int y  = map(p.x, 400, 3800, 0, 240);
    int x  = map(p.y, 3800, 300, 0, 320);
    //Serial.println("Klik " + String(x) + " " + String(y));

    guiRenderer.handleTouch(x, y);
    guiRenderer.render();
  }


  wasTouched = isTouched;

}

}
