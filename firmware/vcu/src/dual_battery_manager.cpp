#include "dual_battery_manager.h"

DualBatteryManager::DualBatteryManager() : _lastPollTimeMs(0) {}

void DualBatteryManager::init() {
    _lastPollTimeMs = millis();
}

void DualBatteryManager::update(BatteryPackState& packA, BatteryPackState& packB, VehicleTelemetry& tel) {
    // Simulasi & Manajemen Dual Baterai Pack Modular
    packA.isConnected = true;
    packB.isConnected = true;
    
    // Jika kedua pack terhubung via Ideal Diode Controller
    if (packA.isConnected && packB.isConnected) {
        tel.packVoltage = (packA.voltage + packB.voltage) / 2.0f;
        tel.packCurrentAmp = packA.currentAmp + packB.currentAmp;
        tel.batterySocPercent = (uint8_t)((packA.socPercent + packB.socPercent) / 2);
        tel.bmsMaxTempC = max(packA.maxTempC, packB.maxTempC);
    } else if (packA.isConnected) {
        tel.packVoltage = packA.voltage;
        tel.packCurrentAmp = packA.currentAmp;
        tel.batterySocPercent = packA.socPercent;
        tel.bmsMaxTempC = packA.maxTempC;
    } else if (packB.isConnected) {
        tel.packVoltage = packB.voltage;
        tel.packCurrentAmp = packB.currentAmp;
        tel.batterySocPercent = packB.socPercent;
        tel.bmsMaxTempC = packB.maxTempC;
    }
}
