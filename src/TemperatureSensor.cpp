#include "TemperatureSensor.h"

TemperatureSensor::TemperatureSensor()
    : thermocouple(MAX_CS, MAX_MOSI, MAX_MISO, MAX_CLK) // CS, DI, DO, CLK – zmień piny wg potrzeb
{
}
float TemperatureSensor::getCJTemperature()
{
    float cjTemp = thermocouple.readCJTemperature();
    // Serial.println("cjtemp: " + String(cjTemp));
    return cjTemp;
}
void TemperatureSensor::begin()
{
    thermocouple.begin();
    thermocouple.setThermocoupleType(MAX31856_TCTYPE_S);
    //thermocouple.setThermocoupleType(MAX31856_TCTYPE_VOLTAGE_GAIN_8);
    //thermocouple.writeRegister8(0x01, 0x88);
    Serial.print(">>>");
    //thermocouple.writeRegister8(0x01, 0x03);
    delay(100); // Czekaj na zapis
    //uint8_t cr1 = thermocouple.readRegister8(0x01);
    //Serial.print("CR1 po zmianie: 0b" );
    //Serial.println(cr1, BIN);
    Serial.println("<<<< " + String(thermocouple.getThermocoupleType()));
    
    float temp;
    do
    {
        delay(400);
        temp = thermocouple.readThermocoupleTemperature();
        lastErrorCode = thermocouple.readFault();
        Serial.println(String(lastErrorCode) + " " + String(temp) );
    } while (lastErrorCode != 0 || isnan(temp) || temp == 0.0f);
    //lastValidTemperature = temp;
    Serial.println("zzzz");
    lastValidTemperature = calcCorrectedTemp(temp); // Ustawiamy ostatnią poprawną temperaturę na temperaturę zimnego złącza
    //lastValidTemperature = temp;
}

void TemperatureSensor::update()
{
    float temp = thermocouple.readThermocoupleTemperature();
    // Serial.print(">> tempProstoZMaxa: " + String(temp));
    lastErrorCode = thermocouple.readFault();

    if (lastErrorCode == 0 && !isnan(temp) && 
    ( abs(lastValidTemperature - temp) < 30.0f || lastValidTemperature == 0))
    {
        //lastValidTemperature = getTemperatureFromRawVoltage(getRawVoltage());
        lastValidTemperature=calcCorrectedTemp(temp);
        //lastValidTemperature = temp;
        errorCount = 0;
    }
    else
    {
        if (SystemState::get().getMode() == SystemMode::Firing)
            handleError();
        SoundManager::playTone(300, 100); // Dźwięk błędu
        Serial.print("Temperature read error: " + String(lastErrorCode) + " Temp: " + String(temp));
    }
}
float TemperatureSensor::calcCorrectedTemp(float wrongTemp){
    //y=0.00004237x^{3}-0.0217994x^{2}+7.40482x-0.043407
    
    float temp = wrongTemp - thermocouple.readCJTemperature();
    //return  0.00004237 * pow(temp,3) -0.0217994*temp*temp+7.40482*temp-0.043407 + thermocouple.readCJTemperature();
    // y=-0.00034378x^{2}+1.40946x+0.989002
    return -0.00034378 * pow(temp, 2) + 1.40946 * temp + 0.989002 + thermocouple.readCJTemperature();

}

void TemperatureSensor::handleError()
{
    errorCount++;
    if (errorCount == 15)
    {
        softResetMAX31856();
        delay(1000); // Czekaj na reset
    }
}
void TemperatureSensor::softResetMAX31856() {
  thermocouple.begin();  // Inicjalizacja SPI + konfiguracja CR0, CR1 itd.
  thermocouple.setThermocoupleType(MAX31856_TCTYPE_S);
}

float TemperatureSensor::getTemperature() const
{
    return lastValidTemperature;
}

int TemperatureSensor::getErrorCount() const
{
    return errorCount;
}

bool TemperatureSensor::hasError() const
{
    return errorCount > 0;
}
/*
float TemperatureSensor::getRawVoltage(){
    uint8_t highByte = thermocouple.readRegister8(0x0C); // LTCBH
    uint8_t lowByte = thermocouple.readRegister8(0x0D);  // LTCBL
    int16_t raw_value = (highByte << 8) | lowByte;
    float voltage_mV = raw_value * 0.0078125f; // MAX31856 ma rozdzielczość 7.8125 µV/LSB
    return voltage_mV;
}*/
/*
float TemperatureSensor::getRawVoltage()
{
    uint8_t byte0 = thermocouple.readRegister8(0x0C); // LTCBH (bity 23:16)
    uint8_t byte1 = thermocouple.readRegister8(0x0D); // LTCBM (bity 15:8)

    // Łączymy tylko 16 MSB (bity 23:8), ignorując 8 LSB (rezerwowe)
    int16_t raw = ((uint16_t)byte0 << 8) | byte1;

    // Konwersja na mV
    float voltage_mV = raw * 0.0078125f;
    //return voltage_mV;
    
    uint8_t cr1 = thermocouple.readRegister8(0x01); // Adres CR1
   // Serial.print("CR1: 0b");
    ///Serial.println(cr1, BIN);
    uint8_t cjth = thermocouple.readRegister8(0x09); // CJTH
    uint8_t cjtl = thermocouple.readRegister8(0x0A); // CJTL
    int16_t cjt = (cjth << 8) | cjtl;
    float cjt_temp = cjt * 0.015625f; // Rozdzielczość 0.015625°C/LSB
    //Serial.print("Cold Junction Temp: ");
    //Serial.println(cjt_temp);
    return cjt_temp;
}
*/
/*
float TemperatureSensor::getRawVoltage()
{
    uint8_t byte0 = thermocouple.readRegister8(0x0C); // LTCBH
    uint8_t byte1 = thermocouple.readRegister8(0x0D); // LTCBM
    uint8_t byte2 = thermocouple.readRegister8(0x0E); // LTCBL

    int32_t raw = ((uint32_t)byte0 << 16) | ((uint32_t)byte1 << 8) | byte2;

    // Jeśli najwyższy bit (bit 23) jest ustawiony – liczba ujemna (signed 24-bit)
    if (raw & 0x800000)
    {
        raw |= 0xFF000000; // Rozszerzenie znaku do 32 bitów
    }

    float voltage_mV = raw * 0.0078125f; // Każdy bit to 7.8125 µV
    Serial.print("Raw: ");
    Serial.print(raw);
    Serial.print(" -> Voltage [mV]: ");
    Serial.println(voltage_mV);
    return voltage_mV;
}

float TemperatureSensor::getTemperatureFromRawVoltage(float rawVoltage)
{
    // Przykładowa konwersja napięcia na temperaturę   y=\left(1.32023\times10^{-9}\right)x^{3}-0.0000254108x^{2}+0.268784x-114.39058
    return (1.32023e-9f * pow(rawVoltage, 3)) - (0.0000254108f * pow(rawVoltage, 2)) + (0.268784f * rawVoltage) - 114.39058f;
}
float TemperatureSensor::readThermocoupleTemperature(){
    return thermocouple.readThermocoupleTemperature();
}
    */
/*
TemperatureSensor::TemperatureSensor()
// CS, DI, DO, CLK – zmień piny wg potrzeb
{
}
float TemperatureSensor::getCJTemperature()
{
    return 20.0f; // Zwracamy domyślną wartość, gdy nie używamy termopary
}
void TemperatureSensor::begin()
{
}

void TemperatureSensor::update()
{
}

void TemperatureSensor::handleError()
{
}

float TemperatureSensor::getTemperature() const
{
    return 30.0f; // Zwracamy domyślną wartość, gdy nie używamy termopary
}

int TemperatureSensor::getErrorCount() const
{
    return 0;
}

bool TemperatureSensor::hasError() const
{
    return false;
}
#endif
*/