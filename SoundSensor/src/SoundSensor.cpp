#include "SoundSensor.h"

// I2C Adresse
#define DNMS_I2C_ADDRESS          0x55

// Kommandos
#define DNMS_CMD_RESET            0x0001
#define DNMS_CMD_READ_VERSION     0x0002
#define DNMS_CMD_CALCULATE_LEQ    0x0003
#define DNMS_CMD_READ_DATA_READY  0x0004
#define DNMS_CMD_READ_LAEQ        0x0005
#define DNMS_CMD_READ_LZEQ        0x0011
#define DNMS_SET_ICS43434         0x001B
#define DNMS_SET_IM72D128         0x001C

// CRC
#define CRC8_POLYNOMIAL           0x31
#define CRC8_INIT                 0xFF

SoundSensor::SoundSensor() {}

void SoundSensor::begin() {
    Wire.begin(PIN_QWIIC_SDA, PIN_QWIIC_SCL);
    Wire.setClock(100000);
    Wire.setTimeout(50);
    _writeCommand(DNMS_SET_IM72D128);
    delay(1000); // Warm-up
}

void SoundSensor::update() {
    _getLeqValues(_laeq, _la_min, _la_max);
}

float SoundSensor::average() {
    return _laeq;
}

float SoundSensor::min() {
    return _la_min;
}

float SoundSensor::max() {
    return _la_max;
}

bool SoundSensor::_getLeqValues(float &leq, float &min, float &max) {
    uint32_t now = millis();
    if (now < _next_calc) {
        return false; 
    } else {
        if (!_writeCommand(DNMS_CMD_CALCULATE_LEQ)) {
            _next_calc = now + _interval_ms;
            return false; 
        } else {
            uint16_t ready = 0;
            uint32_t t0 = millis();

            while (millis() - t0 < 500) {
                if (_readDataReady(DNMS_CMD_READ_DATA_READY, ready) && ready == 1) break;
                delay(20);
            }

            if (ready == 1) {
                if (_readLeqTriplet(DNMS_CMD_READ_LAEQ, leq, min, max)) {
                    // Erfolgreich gelesen
                } else {
                    return false; 
                }
            } else {
                return false; 
            }
            _next_calc = now + _interval_ms;
            return true;
        }
    }
}

bool SoundSensor::_writeCommand(uint16_t cmd) {
    Wire.beginTransmission(DNMS_I2C_ADDRESS);
    Wire.write((uint8_t)(cmd >> 8));
    Wire.write((uint8_t)(cmd & 0xFF));
    uint8_t err = Wire.endTransmission();
    return err == 0;
}

bool SoundSensor::_requestBytes(size_t n, uint8_t *data) {
    size_t got = Wire.requestFrom((int)DNMS_I2C_ADDRESS, (int)n);
    if (got < n) return false;
    for (size_t i = 0; i < n; ++i) {
        data[i] = Wire.read();
    }
    return true;
}

bool SoundSensor::_readDataReady(uint16_t cmd, uint16_t &ready) {
    if (!_writeCommand(cmd)) return false;

    uint8_t buf[3];
    if (!_requestBytes(3, buf)) return false;
    if (_crc8_word(buf) != buf[2]) return false;
    ready = ((uint16_t)buf[0] << 8) | buf[1];
    return true;
}

bool SoundSensor::_readFloat6(const uint8_t *p, float &out) {
    const uint8_t msw[2] = { p[0], p[1] }; const uint8_t crc1 = p[2];
    const uint8_t lsw[2] = { p[3], p[4] }; const uint8_t crc2 = p[5];

    if (_crc8_word(msw) != crc1) return false;
    if (_crc8_word(lsw) != crc2) return false;

    const uint8_t le_bytes[4] = { lsw[1], lsw[0], msw[1], msw[0] };
    memcpy(&out, le_bytes, 4);
    return true;
}

bool SoundSensor::_readLeqTriplet(uint16_t cmd, float &leq, float &min, float &max) {
    if (!_writeCommand(cmd)) return false;
    uint8_t raw[18];
    if (!_requestBytes(sizeof raw, raw)) return false;
    return _readFloat6(raw+0, leq) && _readFloat6(raw+6, min) && _readFloat6(raw+12, max);
}

uint8_t SoundSensor::_crc8_word(const uint8_t *data) {
    uint16_t current_byte;
    uint8_t crc = CRC8_INIT;
    uint8_t crc_bit;
    for (uint8_t i = 0; i < 2; ++i) {
        crc ^= data[i];
        for (crc_bit = 8; crc_bit > 0; --crc_bit) {
            if (crc & 0x80)
                crc = (crc << 1) ^ CRC8_POLYNOMIAL;
            else
                crc = (crc << 1);
        }
    }
    return crc;
}