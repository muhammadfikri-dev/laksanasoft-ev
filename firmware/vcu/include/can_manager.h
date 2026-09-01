#ifndef CAN_MANAGER_H
#define CAN_MANAGER_H

#include <Arduino.h>
#include "driver/twai.h"
#include "config.h"
#include "vcu_state.h"

class CanManager {
public:
    CanManager();
    bool init();
    void broadcastStatus(const VehicleTelemetry& telemetry);
    void checkIncomingMessages(VehicleTelemetry& telemetry);

private:
    bool _isInitialized;
    uint32_t _lastBroadcastTimeMs;
};

#endif // CAN_MANAGER_H
