#include "motor_controller.h"
#include "config.h"

MotorController::MotorController() : 
    _currentTorqueOut(0.0f), 
    _currentRegenOut(0.0f), 
    _isReverseOut(false) {}

void MotorController::init() {
    pinMode(PIN_THROTTLE_OUT, OUTPUT);
    pinMode(PIN_REGEN_BRAKE_OUT, OUTPUT);
    pinMode(PIN_REVERSE_DIR_OUT, OUTPUT);
    
    // Set PWM Frekuensi 10 kHz, 10-bit resolution untuk sinyal analog throttle controller
    ledcSetup(0, 10000, 10);
    ledcAttachPin(PIN_THROTTLE_OUT, 0);
    ledcWrite(0, 0); // Kondisi awal 0V
    
    digitalWrite(PIN_REGEN_BRAKE_OUT, LOW);
    digitalWrite(PIN_REVERSE_DIR_OUT, LOW);
}

void MotorController::setTorqueDemand(float torquePercent, bool isReverse) {
    _currentTorqueOut = constrain(torquePercent, 0.0f, 100.0f);
    _currentRegenOut = 0.0f;
    _isReverseOut = isReverse;
    
    digitalWrite(PIN_REVERSE_DIR_OUT, isReverse ? HIGH : LOW);
    digitalWrite(PIN_REGEN_BRAKE_OUT, LOW);
    
    // Konversi 0 - 100% ke rentang tegangan throttle Inverter (0.8V - 4.2V pada DAC 10-bit)
    // 0.8V / 5.0V * 1023 = 163 (Offset awal)
    // 4.2V / 5.0V * 1023 = 860 (Batas atas)
    uint32_t duty = 0;
    if (_currentTorqueOut > 0.5f) {
        duty = 163 + (uint32_t)((_currentTorqueOut / 100.0f) * (860 - 163));
    }
    ledcWrite(0, duty);
}

void MotorController::setRegenDemand(float regenPercent) {
    _currentTorqueOut = 0.0f;
    _currentRegenOut = constrain(regenPercent, 0.0f, 100.0f);
    
    ledcWrite(0, 0); // Putus daya akselerasi
    
    // Aktifkan sinyal E-Brake ke Inverter
    if (_currentRegenOut > 5.0f) {
        digitalWrite(PIN_REGEN_BRAKE_OUT, HIGH);
    } else {
        digitalWrite(PIN_REGEN_BRAKE_OUT, LOW);
    }
}

void MotorController::emergencyStop() {
    _currentTorqueOut = 0.0f;
    _currentRegenOut = 0.0f;
    ledcWrite(0, 0);
    digitalWrite(PIN_REGEN_BRAKE_OUT, LOW);
    digitalWrite(PIN_REVERSE_DIR_OUT, LOW);
}

void MotorController::updateFeedback(VehicleTelemetry& telemetry) {
    // Estimasi dinamika kecepatan & RPM motor berbasis daya / feedback inverter
    // Rasio reduksi 1:4.2, keliling ban 14 inci = 1.677 meter
    if (telemetry.currentState == STATE_DRIVE) {
        float targetRpm = (_currentTorqueOut / 100.0f) * 4500.0f;
        telemetry.motorRpm = (int16_t)((telemetry.motorRpm * 0.9f) + (targetRpm * 0.1f));
    } else if (telemetry.currentState == STATE_REGEN) {
        telemetry.motorRpm = (int16_t)(telemetry.motorRpm * 0.85f);
    } else {
        telemetry.motorRpm = 0;
    }
    
    // Kecepatan km/h = (RPM Motor / 4.2) * 1.677m * 60 / 1000
    telemetry.vehicleSpeedKmh = (telemetry.motorRpm / 4.2f) * 1.677f * 0.06f;
    if (telemetry.vehicleSpeedKmh < 0.5f) telemetry.vehicleSpeedKmh = 0.0f;
    
    // Simulasi suhu operasional termal
    if (telemetry.motorTempC == 0.0f) telemetry.motorTempC = 42.0f;
    if (telemetry.inverterTempC == 0.0f) telemetry.inverterTempC = 38.0f;
    
    if (_currentTorqueOut > 50.0f) {
        telemetry.motorTempC += 0.02f;
        telemetry.inverterTempC += 0.015f;
    } else {
        if (telemetry.motorTempC > 40.0f) telemetry.motorTempC -= 0.01f;
        if (telemetry.inverterTempC > 36.0f) telemetry.inverterTempC -= 0.01f;
    }
}
