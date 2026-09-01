#ifndef THROTTLE_HANDLER_H
#define THROTTLE_HANDLER_H

#include <Arduino.h>
#include "config.h"
#include "vcu_state.h"

class ThrottleHandler {
public:
    ThrottleHandler();
    void init();
    
    // Membaca ADC APPS1 & APPS2, melakukan validasi plausibilitas, dan menghitung persentase bukaan gas
    bool process(float& outThrottlePercent, float& outApps1Volt, float& outApps2Volt, FaultCode& outFault);
    
    // Menerapkan kurva pemetaan torsi berdasarkan mode berkendara (ECO, URBAN, SPORT, REVERSE)
    float mapTorqueToMode(float validThrottlePercent, DriveMode mode);

private:
    uint32_t _mismatchStartTimeMs;
    bool _isMismatched;
    float _filteredPercent;
    
    float readChannelVoltage(uint8_t pin);
};

#endif // THROTTLE_HANDLER_H
