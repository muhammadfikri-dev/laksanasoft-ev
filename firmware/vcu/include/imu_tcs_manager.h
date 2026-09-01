#ifndef IMU_TCS_MANAGER_H
#define IMU_TCS_MANAGER_H

#include <Arduino.h>
#include "vcu_state.h"

class ImuTcsManager {
public:
    ImuTcsManager();
    bool init();
    
    // Membaca sensor IMU 6-DOF (Roll, Pitch, Akselerasi G)
    void update(float& outRollDeg, float& outPitchDeg, bool& outCrashDetected);
    
    // Algoritma Kendali Traksi (TCS): Menghitung reduksi torsi jika terdeteksi slip roda
    float processTractionControl(float requestedTorque, float currentSpeedKmh, int16_t motorRpm, float leanAngleDeg, bool& outTcsActive);

private:
    float _rollAngle;
    float _pitchAngle;
    uint32_t _lastTimeMs;
    int16_t _lastRpm;
};

#endif // IMU_TCS_MANAGER_H
