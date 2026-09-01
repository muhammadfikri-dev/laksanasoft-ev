#ifndef KEYLESS_MANAGER_H
#define KEYLESS_MANAGER_H

#include <Arduino.h>

enum KeylessAuthMethod {
    AUTH_NONE = 0,
    AUTH_NFC_CARD,
    AUTH_BLE_PROXIMITY,
    AUTH_PIN_CODE,
    AUTH_MOBILE_APP
};

class KeylessManager {
public:
    KeylessManager();
    void init();
    
    // Memeriksa status otentikasi kunci digital
    bool isAuthorized() const { return _isUnlocked; }
    KeylessAuthMethod getAuthMethod() const { return _activeMethod; }
    
    // Aksi kontrol
    void unlockWithNfc(const String& cardUid);
    void unlockWithBleProximity(int rssiDbm);
    bool unlockWithPin(const String& inputPin);
    void lockVehicle();

    void update();

private:
    bool _isUnlocked;
    KeylessAuthMethod _activeMethod;
    uint32_t _lastAuthTimeMs;
    String _storedPin;
};

#endif // KEYLESS_MANAGER_H
