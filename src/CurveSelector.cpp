#include "CurveSelector.h"
#include "StorageManager.h"

CurveSelector::CurveSelector(CurveManager &manager)
    : curveManager(manager), selectedIndex(0)
{
    (StorageManager::loadCurve(manager, 0));
}

void CurveSelector::selectNext()
{
    selectedIndex = (selectedIndex + 1);
    // Serial.println("Selected index: " + String(selectedIndex));
    (StorageManager::loadCurve(curveManager, selectedIndex));
}

void CurveSelector::selectPrevious()
{
    selectedIndex = selectedIndex <= 1 ? 1 : (selectedIndex - 1);

    (StorageManager::loadCurve(curveManager, selectedIndex));
}

int CurveSelector::getSelectedIndex() const
{
    return selectedIndex;
}

void CurveSelector::selectByIndex(int index)
{
    if (index < 1)
        index = 1;
    (StorageManager::loadCurve(curveManager, index));
}