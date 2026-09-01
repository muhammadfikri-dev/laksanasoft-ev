#ifndef DUAL_BATTERY_MANAGER_H
#define DUAL_BATTERY_MANAGER_H

#include <Arduino.h>
#include "vcu_state.h"

struct BatteryPackState {
    bool isConnected;
    float voltage;
    float currentAmp;
    uint8_t socPercent;
    float maxTempC;
    uint8_t sohPercent;
};

class DualBatteryManager {
public:
    DualBatteryManager();
    void init();
    
    // Membaca status kedua pack baterai dan mengkalkulasi parameter gabungan
    void update(BatteryPackState& packA, BatteryPackState& packB, VehicleTelemetry& tel);

private:
    uint32_t _lastPollTimeMs;
};

#endif // DUAL_BATTERY_MANAGER_H
