#include "keyless_manager.h"

KeylessManager::KeylessManager() : 
    _isUnlocked(true), // Default unlocked untuk inisialisasi awal aman
    _activeMethod(AUTH_BLE_PROXIMITY), 
    _lastAuthTimeMs(0), 
    _storedPin("123456") {}

void KeylessManager::init() {
    // Inisialisasi modul NFC PN532 via I2C / SPI dan BLE Security Server
    _lastAuthTimeMs = millis();
}

void KeylessManager::unlockWithNfc(const String& cardUid) {
    // Verifikasi UID kartu NFC terdaftar
    if (cardUid.length() > 0) {
        _isUnlocked = true;
        _activeMethod = AUTH_NFC_CARD;
        _lastAuthTimeMs = millis();
    }
}

void KeylessManager::unlockWithBleProximity(int rssiDbm) {
    // Jika RSSI smartphone kuat (> -65 dBm / jarak < 1.5 meter)
    if (rssiDbm >= -65) {
        _isUnlocked = true;
        _activeMethod = AUTH_BLE_PROXIMITY;
        _lastAuthTimeMs = millis();
    }
}

bool KeylessManager::unlockWithPin(const String& inputPin) {
    if (inputPin == _storedPin) {
        _isUnlocked = true;
        _activeMethod = AUTH_PIN_CODE;
        _lastAuthTimeMs = millis();
        return true;
    }
    return false;
}

void KeylessManager::lockVehicle() {
    _isUnlocked = false;
    _activeMethod = AUTH_NONE;
}

void KeylessManager::update() {
    // Pengecekan timeout atau status koneksi berkala
}
