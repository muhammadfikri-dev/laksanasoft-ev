#include "throttle_handler.h"
#include <math.h>

ThrottleHandler::ThrottleHandler() : 
    _mismatchStartTimeMs(0), 
    _isMismatched(false), 
    _filteredPercent(0.0f) {}

void ThrottleHandler::init() {
    pinMode(PIN_APPS_1, INPUT);
    pinMode(PIN_APPS_2, INPUT);
    analogReadResolution(12); // Resolusi 12-bit ADC ESP32-S3 (0 - 4095)
}

float ThrottleHandler::readChannelVoltage(uint8_t pin) {
    // Membaca nilai ADC dan konversi ke voltase (dengan referensi 3.3V dan pembagi tegangan 5V->3.3V)
    uint32_t rawSum = 0;
    const uint8_t SAMPLES = 8;
    for (uint8_t i = 0; i < SAMPLES; i++) {
        rawSum += analogRead(pin);
    }
    float rawAvg = (float)rawSum / SAMPLES;
    
    // Asumsi voltage divider rasio 5V sensor ke 3.3V ADC: Faktor konversi = (3.3 / 4095.0) * (5.0 / 3.3)
    float voltage = (rawAvg / 4095.0f) * 5.0f;
    return voltage;
}

bool ThrottleHandler::process(float& outThrottlePercent, float& outApps1Volt, float& outApps2Volt, FaultCode& outFault) {
    outApps1Volt = readChannelVoltage(PIN_APPS_1);
    outApps2Volt = readChannelVoltage(PIN_APPS_2);
    
    // 1. Validasi Batas Tegangan Fisik (Range Out-of-Bounds Check)
    if (outApps1Volt < APPS_VOLT_MIN_VALID || outApps1Volt > APPS_VOLT_MAX_VALID ||
        outApps2Volt < APPS_VOLT_MIN_VALID || outApps2Volt > APPS_VOLT_MAX_VALID) {
        outFault = FAULT_APPS_OUT_OF_RANGE;
        outThrottlePercent = 0.0f;
        _filteredPercent = 0.0f;
        return false;
    }
    
    // 2. Normalisasi Tegangan ke Persentase (0.8V = 0%, 4.2V = 100%)
    float p1 = (outApps1Volt - 0.8f) / (4.2f - 0.8f) * 100.0f;
    float p2 = (4.2f - outApps2Volt) / (4.2f - 0.8f) * 100.0f; // Sinyal inversi pada APPS2
    
    p1 = constrain(p1, 0.0f, 100.0f);
    p2 = constrain(p2, 0.0f, 100.0f);
    
    // 3. Pengecekan Diskrepansi / Mismatch Sensor
    float discrepancy = fabs(p1 - p2);
    if (discrepancy > APPS_MAX_DISCREPANCY) {
        if (!_isMismatched) {
            _isMismatched = true;
            _mismatchStartTimeMs = millis();
        } else if (millis() - _mismatchStartTimeMs > APPS_FAULT_TIMEOUT_MS) {
            outFault = FAULT_APPS_MISMATCH;
            outThrottlePercent = 0.0f;
            _filteredPercent = 0.0f;
            return false; // Potong daya gas seketika
        }
    } else {
        _isMismatched = false;
    }
    
    // 4. Kalkulasi Rata-rata & Deadband
    float rawThrottle = (p1 + p2) / 2.0f;
    if (rawThrottle <= APPS_DEADBAND_PERCENT) {
        rawThrottle = 0.0f;
    } else {
        // Skala ulang sisa rentang dari deadband hingga 100%
        rawThrottle = ((rawThrottle - APPS_DEADBAND_PERCENT) / (100.0f - APPS_DEADBAND_PERCENT)) * 100.0f;
    }
    
    // 5. Low-Pass Filter Eksponensial untuk Kehalusan Sinyal
    _filteredPercent = (_filteredPercent * 0.7f) + (rawThrottle * 0.3f);
    outThrottlePercent = constrain(_filteredPercent, 0.0f, 100.0f);
    outFault = FAULT_NONE;
    return true;
}

float ThrottleHandler::mapTorqueToMode(float validThrottlePercent, DriveMode mode) {
    if (validThrottlePercent <= 0.0f) return 0.0f;
    
    float norm = validThrottlePercent / 100.0f;
    float targetTorque = 0.0f;
    
    switch (mode) {
        case MODE_ECO:
            // Kurva kuadratik halus, dibatasi pada 40% daya maksimum
            targetTorque = 40.0f * (norm * norm);
            break;
            
        case MODE_URBAN:
            // Karakter linear moderat, dibatasi pada 75% daya maksimum
            targetTorque = 75.0f * norm;
            break;
            
        case MODE_SPORT:
            // Respon instan penuh 100% daya
            targetTorque = 100.0f * norm;
            break;
            
        case MODE_REVERSE:
            // Mode mundur kecepatan lambat, dibatasi 15%
            targetTorque = 15.0f * norm;
            break;
            
        default:
            targetTorque = 0.0f;
            break;
    }
    
    return constrain(targetTorque, 0.0f, 100.0f);
}
