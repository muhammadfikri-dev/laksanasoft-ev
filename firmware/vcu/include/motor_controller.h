#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include <Arduino.h>
#include "vcu_state.h"

class MotorController {
public:
    MotorController();
    void init();
    
    // Mengirim perintah daya/torsi ke Inverter via PWM/DAC atau CAN Bus
    void setTorqueDemand(float torquePercent, bool isReverse);
    
    // Mengirim perintah torsi pengereman regeneratif
    void setRegenDemand(float regenPercent);
    
    // Pemutusan seketika (Safe Torque Off)
    void emergencyStop();
    
    // Update feedback dinamika kendaraan (kecepatan, rpm, suhu)
    void updateFeedback(VehicleTelemetry& telemetry);

private:
    float _currentTorqueOut;
    float _currentRegenOut;
    bool _isReverseOut;
};

#endif // MOTOR_CONTROLLER_H
