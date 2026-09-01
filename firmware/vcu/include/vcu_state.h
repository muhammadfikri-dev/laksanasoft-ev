#ifndef VCU_STATE_H
#define VCU_STATE_H

#include <Arduino.h>

// Status Utama Kendaraan (Vehicle Operational States)
enum VehicleState {
    STATE_BOOT = 0,         // Inisialisasi hardware & Power-On Self-Test (POST)
    STATE_PRECHARGE,        // Pengisian kapasitor inverter (1.5 detik)
    STATE_STANDBY,          // Sistem siap, menunggu standar samping & rem
    STATE_DRIVE,            // Sistem aktif, motor dapat digas
    STATE_REGEN,            // Pengereman regeneratif aktif
    STATE_FAULT,            // Terjadi kesalahan keamanan (interlock trip)
    STATE_CHARGING,         // Pengisian daya baterai terdeteksi
    STATE_SHUTDOWN          // Pemutusan daya aman
};

// Mode Berkendara (Drive Modes)
enum DriveMode {
    MODE_ECO = 0,           // Hemat daya, maks 50 km/jam, 35% torsi
    MODE_URBAN,             // Harian seimbang, maks 85 km/jam, 70% torsi
    MODE_SPORT,             // Performa penuh 120+ km/jam (Active Flux Weakening)
    MODE_REVERSE            // Gigi mundur aman, maks 5 km/jam
};

// Kode Kesalahan Failsafe (Fault Codes)
enum FaultCode {
    FAULT_NONE = 0,
    FAULT_APPS_MISMATCH,    // Deviasi APPS1 & APPS2 > 10%
    FAULT_APPS_OUT_OF_RANGE,// Sinyal ADC di luar batas valid (kabel putus/korslet)
    FAULT_BRAKE_PRIORITY,   // Gas & rem ditekan bersamaan (BOA interlock)
    FAULT_BMS_UNDERVOLTAGE, // Tegangan baterai di bawah 60.0V
    FAULT_BMS_OVERVOLTAGE,  // Tegangan baterai di atas 84.5V
    FAULT_OVER_TEMPERATURE, // Suhu motor atau inverter melebihi batas aman (>95°C)
    FAULT_CAN_TIMEOUT,      // Hilang komunikasi dengan inverter / BMS
    FAULT_EMERGENCY_STOP,   // Tombol darurat aktif
    FAULT_CRASH_TIPOVER     // Sensor IMU mendeteksi motor terjatuh (>60°)
};

// Struktur Data Telemetri & Status Kendaraan Lengkap (Smart VCU 2.0)
struct VehicleTelemetry {
    // Status Sistem & Keamanan Cerdas
    VehicleState currentState;
    DriveMode currentMode;
    FaultCode activeFault;
    bool isReady;
    bool isSideStandDown;
    bool isBrakeEngaged;
    bool isReverseActive;
    bool isTcsActive;            // Status Intervensi Traction Control
    bool isKeylessUnlocked;      // Status Otentikasi Smart Keyless (NFC/BLE)
    
    // Sensor Inersia 6-Axis (IMU)
    float leanAngleDeg;          // Sudut Kemiringan Menikung (Roll)
    float pitchAngleDeg;         // Sudut Tanjakan/Turunan (Pitch)
    
    // Input Pengendali
    float throttlePercent;       // Nilai gas terhitung (0.0 - 100.0%)
    float apps1Voltage;
    float apps2Voltage;
    
    // Powertrain & Dinamika (120 km/jam Scale)
    float vehicleSpeedKmh;
    int16_t motorRpm;
    float phaseCurrentAmp;
    float targetTorquePercent;
    
    // Sistem Energi Ganda (Dual Smart Battery Architecture)
    float packVoltage;
    float packCurrentAmp;
    float packPowerWatts;
    uint8_t batterySocPercent;
    uint8_t batterySohPercent;
    
    uint8_t packASoc;            // Pack A Modular SOC (%)
    uint8_t packBSoc;            // Pack B Modular SOC (%)
    float packAVoltage;
    float packBVoltage;
    
    float cellMaxVoltage;
    float cellMinVoltage;
    uint16_t cellImbalanceMv;
    
    // Suhu Sensorik (°C)
    float motorTempC;
    float inverterTempC;
    float bmsMaxTempC;
    float ambientTempC;
    
    // Metrik Perjalanan & Navigasi
    float tripDistanceKm;
    float totalOdometerKm;
    float estimatedRangeKm;
    float energyEfficiencyWhPerKm;
    
    // Waktu Operasi
    uint32_t uptimeSeconds;
};

// Helper untuk konversi enum ke teks bahasa Indonesia
inline const char* getVehicleStateString(VehicleState s) {
    switch (s) {
        case STATE_BOOT: return "BOOT / SELF-TEST";
        case STATE_PRECHARGE: return "PRE-CHARGE AKTIF";
        case STATE_STANDBY: return "STANDBY";
        case STATE_DRIVE: return "DRIVE (READY)";
        case STATE_REGEN: return "E-REGEN BRAKING";
        case STATE_FAULT: return "FAULT / INTERLOCK";
        case STATE_CHARGING: return "CHARGING";
        case STATE_SHUTDOWN: return "SHUTDOWN";
        default: return "UNKNOWN";
    }
}

inline const char* getDriveModeString(DriveMode m) {
    switch (m) {
        case MODE_ECO: return "ECO (50 KM/H)";
        case MODE_URBAN: return "URBAN (85 KM/H)";
        case MODE_SPORT: return "SPORT (120+ KM/H)";
        case MODE_REVERSE: return "REVERSE (5 KM/H)";
        default: return "UNKNOWN";
    }
}

inline const char* getFaultCodeString(FaultCode f) {
    switch (f) {
        case FAULT_NONE: return "NORMAL";
        case FAULT_APPS_MISMATCH: return "ERR_01: APPS SENSOR MISMATCH";
        case FAULT_APPS_OUT_OF_RANGE: return "ERR_01B: APPS SIGNAL OUT OF RANGE";
        case FAULT_BRAKE_PRIORITY: return "ERR_02: BRAKE OVER ACCELERATOR";
        case FAULT_BMS_UNDERVOLTAGE: return "ERR_04: BATT LOW VOLTAGE CUTOFF";
        case FAULT_BMS_OVERVOLTAGE: return "ERR_04B: BATT OVERVOLTAGE";
        case FAULT_OVER_TEMPERATURE: return "ERR_06: THERMAL OVERHEAT TRIP (>95C)";
        case FAULT_CAN_TIMEOUT: return "ERR_07: CAN BUS COMMS TIMEOUT";
        case FAULT_EMERGENCY_STOP: return "ERR_08: EMERGENCY STOP TRIGGERED";
        case FAULT_CRASH_TIPOVER: return "ERR_09: CRASH / TIP-OVER SENSOR TRIP";
        default: return "ERR_UNKNOWN";
    }
}

#endif // VCU_STATE_H
