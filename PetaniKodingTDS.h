/***************************************************
 Ini adalah library custom yang saya buat dari Library GravityTDS
 Kalibrasi dengan input external
 Created By PetaniKoding
 ****************************************************/

#ifndef PetaniKodingTDS_H
#define PetaniKodingTDS_H

#include <Arduino.h>
#include <EEPROM.h>

class PetaniKodingTDS
{
public:
    PetaniKodingTDS();

    void setPin(int pin);
    void setAref(float value);
    void setAdcRange(int range);
    void setTemperature(float temp);
    void begin();
    void update();
    float getTdsValue();
    void beginCalibration();                  // masuk mode kalibrasi
    bool setCalibration(float tds);    // kalibrasi pakai nilai TDS
    void endCalibration();                    // simpan & keluar

private:
    float getMedianNum(int bArray[], int iFilterLen);

private:
    int pin;
    float aref;
    int adcRange;
    float temperature;
    float tdsValue;
    float ecValue;
    float voltage;
    float kValue;

    static const int TdsFactor = 2;
    static const int kValueAddress = 8;

    int analogBuffer[30];
    int analogBufferTemp[30];
    int analogBufferIndex;
};

#endif
