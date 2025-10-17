#ifndef SoundSensor_h
#define SoundSensor_h

#include <Arduino.h>
#include <Wire.h>

class SoundSensor {
public:
    SoundSensor();
    void begin();
    void update();
    int average();
    int min();
    int max();

private:
    float _laeq = 0.0;
    float _la_min = 0.0;
    float _la_max = 0.0;
    uint32_t _next_calc = 0;
    static const uint32_t _interval_ms = 1000;

    bool _getLeqValues(float &leq, float &min, float &max);
    bool _writeCommand(uint16_t cmd);
    bool _requestBytes(size_t n, uint8_t *data);
    bool _readDataReady(uint16_t cmd, uint16_t &ready);
    bool _readFloat6(const uint8_t *p, float &out);
    bool _readLeqTriplet(uint16_t cmd, float &leq, float &min, float &max);
    uint8_t _crc8_word(const uint8_t *data);
};

#endif