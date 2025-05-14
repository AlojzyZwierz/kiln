#ifndef CURVE_SELECTOR_H
#define CURVE_SELECTOR_H

#include "CurveManager.h"

class CurveSelector {
public:
    CurveSelector(CurveManager& manager);
    void selectByIndex(int index) ;
    void selectNext();
    void selectPrevious();
    int getSelectedIndex() const;

private:
    CurveManager& curveManager;
    int selectedIndex = 0;
};

#endif
