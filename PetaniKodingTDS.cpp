/***************************************************
 Ini adalah library custom yang saya buat dari Library GravityTDS
 Kalibrasi dengan input external
 Created By PetaniKoding
 ****************************************************/

#include "PetaniKodingTDS.h"

PetaniKodingTDS::PetaniKodingTDS()
{
    pin = A1;
    aref = 5.0;
    adcRange = 1024;
    temperature = 25.0;
    analogBufferIndex = 0;
}

static bool enterCalibrationFlag = false;
static bool ecCalibrationFinish = false;

void PetaniKodingTDS::setPin(int pin)
{
    this->pin = pin;
}

void PetaniKodingTDS::setAref(float value)
{
    aref = value;
}

void PetaniKodingTDS::setAdcRange(int range)
{
    adcRange = range;
}

void PetaniKodingTDS::setTemperature(float temp)
{
    temperature = temp;
}

void PetaniKodingTDS::begin()
{
    EEPROM.get(kValueAddress, kValue);
    if (isnan(kValue) || kValue <= 0)
        kValue = 1.0;
}

void PetaniKodingTDS::update()
{
    analogBuffer[analogBufferIndex++] = analogRead(pin);
    if (analogBufferIndex == 30)
        analogBufferIndex = 0;

    for (int i = 0; i < 30; i++)
        analogBufferTemp[i] = analogBuffer[i];

    float averageVoltage =
        getMedianNum(analogBufferTemp, 30) * aref / adcRange;

    voltage = averageVoltage;

    ecValue =
        (133.42 * voltage * voltage * voltage
        - 255.86 * voltage * voltage
        + 857.39 * voltage) * kValue;

    ecValue = ecValue / (1.0 + 0.02 * (temperature - 25.0));

    tdsValue = ecValue * TdsFactor;
}

float PetaniKodingTDS::getTdsValue()
{
    return tdsValue;
}

// ======================
// KALIBRASI KEYPAD
// ======================

void PetaniKodingTDS::beginCalibration()
{
    enterCalibrationFlag = true;
    ecCalibrationFinish = false;
}

bool PetaniKodingTDS::setCalibration(float knownTDS)
{
    if (!enterCalibrationFlag) return false;

    float rawECsolution = knownTDS / (float)TdsFactor;
    rawECsolution *= (1.0 + 0.02 * (temperature - 25.0));

    float KValueTemp =
        rawECsolution /
        (133.42 * voltage * voltage * voltage
        - 255.86 * voltage * voltage
        + 857.39 * voltage);

    if ((rawECsolution > 0 && rawECsolution < 2000) &&
        (KValueTemp > 0.25 && KValueTemp < 4.0))
    {
        kValue = KValueTemp;
        ecCalibrationFinish = true;
        return true;
    }

    ecCalibrationFinish = false;
    return false;
}

void PetaniKodingTDS::endCalibration()
{
    if (enterCalibrationFlag && ecCalibrationFinish)
    {
        EEPROM.put(kValueAddress, kValue);
    }

    enterCalibrationFlag = false;
    ecCalibrationFinish = false;
}

// ======================
// MEDIAN FILTER
// ======================

float PetaniKodingTDS::getMedianNum(int bArray[], int iFilterLen)
{
    int bTab[iFilterLen];
    for (int i = 0; i < iFilterLen; i++)
        bTab[i] = bArray[i];

    for (int j = 0; j < iFilterLen - 1; j++)
    {
        for (int i = 0; i < iFilterLen - j - 1; i++)
        {
            if (bTab[i] > bTab[i + 1])
            {
                int temp = bTab[i];
                bTab[i] = bTab[i + 1];
                bTab[i + 1] = temp;
            }
        }
    }

    if ((iFilterLen & 1) > 0)
        return bTab[(iFilterLen - 1) / 2];
    else
        return (bTab[iFilterLen / 2] + bTab[iFilterLen / 2 - 1]) / 2;
}
