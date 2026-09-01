#include "imu_tcs_manager.h"
#include <math.h>

ImuTcsManager::ImuTcsManager() : 
    _rollAngle(0.0f), 
    _pitchAngle(0.0f), 
    _lastTimeMs(0), 
    _lastRpm(0) {}

bool ImuTcsManager::init() {
    // Inisialisasi antarmuka I2C sensor IMU (MPU6050 / ICM-42688 6-Axis)
    _lastTimeMs = millis();
    return true;
}

void ImuTcsManager::update(float& outRollDeg, float& outPitchDeg, bool& outCrashDetected) {
    // Simulasi & integrasi filter komplementer IMU 6-DOF
    // Dalam implementasi nyata: I2C read accelerometer (ax, ay, az) & gyroscope (gx, gy, gz)
    outRollDeg = _rollAngle;
    outPitchDeg = _pitchAngle;
    
    // Deteksi Crash / Tip-Over: Kemiringan ekstrem > 60 derajat
    outCrashDetected = (fabs(_rollAngle) > 60.0f);
}

float ImuTcsManager::processTractionControl(float requestedTorque, float currentSpeedKmh, int16_t motorRpm, float leanAngleDeg, bool& outTcsActive) {
    outTcsActive = false;
    if (requestedTorque <= 0.0f) return 0.0f;
    
    uint32_t now = millis();
    float dt = (now - _lastTimeMs) / 1000.0f;
    if (dt <= 0.001f) dt = 0.01f;
    _lastTimeMs = now;
    
    // 1. Hitung Akselerasi Putaran Roda (dRPM/dt)
    float deltaRpm = (motorRpm - _lastRpm) / dt;
    _lastRpm = motorRpm;
    
    // Estimasi RPM teoretis berdasarkan kecepatan linear kendaraan (Rasio 1:3.0)
    // RPM = (Speed km/h / 0.06) / 1.677 * 3.0
    float expectedRpm = (currentSpeedKmh / 0.06f / 1.677f) * 3.0f;
    float rpmSlip = motorRpm - expectedRpm;
    
    float safeTorque = requestedTorque;

    // 2. Deteksi Roda Selip (Wheel Slip / Wheelspin Terdeteksi)
    if (rpmSlip > 600.0f || deltaRpm > 3500.0f) {
        outTcsActive = true;
        // Reduksi torsi instan sebesar 40% untuk mengembalikan traksi ban ke aspal
        safeTorque *= 0.60f;
    }
    
    // 3. Cornering Lean Angle Safety: Batasi torsi agresif saat motor miring > 35 derajat
    float absLean = fabs(leanAngleDeg);
    if (absLean > 35.0f) {
        float leanReductionFactor = 1.0f - ((absLean - 35.0f) / 25.0f); // 35° -> 100%, 55° -> 20%
        leanReductionFactor = constrain(leanReductionFactor, 0.20f, 1.0f);
        safeTorque *= leanReductionFactor;
    }
    
    return safeTorque;
}
