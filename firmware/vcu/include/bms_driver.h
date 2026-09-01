#ifndef BMS_DRIVER_H
#define BMS_DRIVER_H

#include <Arduino.h>
#include "vcu_state.h"

class BmsDriver {
public:
    BmsDriver();
    void init();
    bool update(VehicleTelemetry& telemetry);

private:
    uint32_t _lastQueryTimeMs;
    uint32_t _lastValidPacketTimeMs;
    
    void requestBmsData();
    bool parseBmsFrame(const uint8_t* buffer, size_t length, VehicleTelemetry& telemetry);
};

#endif // BMS_DRIVER_H
