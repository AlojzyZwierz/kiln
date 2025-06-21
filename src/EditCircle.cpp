#include <EditCircle.h>
#include <arduino.h>
#include <ColorPalette.h>

EditCircle::EditCircle(int x, int y, int radius, CurveManager &cm)
    : posX(x), posY(y), radius(radius), curveManager(cm)
{
    active = true;
    highlighted = false;
    visible = false;

    // curve = curveManager.getOriginalCurve();
}

void EditCircle::render(TFT_eSprite &tft)
{
    uint16_t color = COLOR_EDIT_CIRCLE;
    tft.drawCircle(posX, posY, radius, color);
    tft.drawLine(posX - radius * 0.7071f, posY - radius * 0.7071f, posX + radius * 0.7071f, posY + radius * 0.7071f, color);
    tft.drawLine(posX + radius * 0.7071f, posY - radius * 0.7071f, posX - radius * 0.7071f, posY + radius * 0.7071f, color);
    tft.drawCircle(posX, posY, 20, color);
}

bool EditCircle::wasClicked(int x, int y)
{
    int dx = x - posX;
    int dy = y - posY;
    return std::sqrt(dx * dx + dy * dy) <= radius;
}

float EditCircle::mapValue(float val, float inMin, float inMax, float outMin, float outMax)
{
    val = std::max(inMin, std::min(val, inMax));
    return outMin + (outMax - outMin) * ((val - inMin) / (inMax - inMin));
}

bool EditCircle::handleClick(int x, int y)
{
    if (!visible || !wasClicked(x, y))
        return false;
    // curve = curveManager.getOriginalCurve();
    int dx = x - posX;
    int dy = y - posY;

    int absDx = std::abs(dx);
    int absDy = std::abs(dy);
    float distance = std::sqrt(dx * dx + dy * dy);
    float factor = distance < 20 ? 1 : 0.204 * distance * distance - 8.4 * distance + 88.4; // y = 0.0528571 * x^2 - 0.0285714 * x + 0.0285714; // y = 0.0528571 * x^2 - 0.0285714 * x + 0.0285714; // y = 0.0528571 * x^2 - 0.0285714 * x + 0.0285714; // y = 0.0528571 * x^2 - 0.0285714 * x + 0.0285714; // y = 0.0528571 * x^2 - 0.0285714 * x + 0.0285714; // y = 0.0528571 * x^2 - 0.0285714 * x + 0.0285714; // y = 0.0528571 * x^2 - 0.0285714 * x + 0.0285714; // y = 0.0528571 * x^2 - 0.0285714 * x + 0.0285714; // y = 0.0528571 * x^2 - 0.0285714 * x + 0.0285714; // y = 0.0528571 * x^2 - 0.0285714 * x + 0.0285714; // y = −
    // y = 88.4 - 8.4*x + 0.204*x^2
    if (absDx > absDy)
    {
        int sign = dx > 0 ? 1 : -1;
        long newTime;
        if (factor != 1)
        {
            factor = 6000 * factor;

            newTime = ((curveManager.getSegmentTime() + sign * (int)factor));
            // newTime = (newTime /100)*1000;

           // Serial.println("newTime1: " + String(newTime) + " factor: " + String((int)factor) + " oldtime: " + String(curveManager.getSegmentTime()));

            //Serial.println("newTime2: " + String(newTime));

            newTime = std::max(newTime - (newTime % 60000), (long)60000); // Ensure minimum time is 60 seconds
        }
        else
        {
            newTime = curveManager.getSegmentTime() + sign * 60000;
            if (newTime < 60000)
                newTime = 60000; // Ensure minimum time is 60 seconds
        }
        curveManager.updateTime(curveManager.getSegmentIndex(), newTime);

        //  {curve.elems[curveManager.getSegmentIndex()].hTime = std::max<long>(60000, curve.elems[curveManager.getSegmentIndex()].hTime + (int)factor ) ;}
        // {curve.elems[curveManager.getSegmentIndex()].hTime = std::max<long>(60000, curve.elems[curveManager.getSegmentIndex()].hTime - (int)factor);}
        //  Serial.println("hTime: " + String(curve.elems[curveManager.getSegmentIndex()].hTime) + " factor: " + String((int)factor) + " dx: " + String(dx) + " dy: " + String(dy) );
    }
    else
    {
        int sign = dy < 0 ? 1 : -1;
        float newTemp = curveManager.getSegmentTemp() + sign * (int)factor;
        if (newTemp < 20)
            newTemp = 20;
        if (newTemp > 1300)
            newTemp = 1300;
        curveManager.updateTemperature(curveManager.getSegmentIndex(), newTemp);
        // if (dy > 0) curve.elems[curveManager.getSegmentIndex()].endTemp = std::min<float>(1300, curve.elems[curveManager.getSegmentIndex()].endTemp - (int)factor);
        // else curve.elems[curveManager.getSegmentIndex()].endTemp = std::max<float>(20, curve.elems[curveManager.getSegmentIndex()].endTemp + (int)factor);
        //  Serial.println("endTemp: " + String(curve.elems[curveManager.getSegmentIndex()].endTemp) + " factor: " + String((int)factor) + " dx: " + String(dx) + " dy: " + String(dy)+ " distance: " + String(distance) );
    }
    // curveManager.loadOriginalCurve(curve);
    return true;
}
