#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ==========================================
// IDENTIFIKASI KENDARAAN (VEHICLE ID)
// ==========================================
#define VEHICLE_VIN             "LAKSANAEV2026ID001"
#define FIRMWARE_VERSION        "1.0.0-PROD"

// ==========================================
// ALOKASI PIN GPIO (ESP32-S3)
// ==========================================
// Input Analog Throttle Dual-Channel
#define PIN_APPS_1              1   // ADC1_CH0 (Sinyal Gas 1: 0.8V - 4.2V)
#define PIN_APPS_2              2   // ADC1_CH1 (Sinyal Gas 2 Inverted: 4.2V - 0.8V)

// Input Digital Sensor & Interlock
#define PIN_BRAKE_INPUT         4   // Sensor Tuas Rem (Active LOW)
#define PIN_SIDE_STAND          5   // Sensor Standar Samping (Active LOW)
#define PIN_MODE_BUTTON         6   // Tombol Mode Berkendara (Active LOW)
#define PIN_REVERSE_SWITCH      7   // Saklar Mundur (Active LOW)

// Output Kendali Daya (MOSFET Relay Driver)
#define PIN_MAIN_CONTACTOR      8   // Kontaktor Utama 72V
#define PIN_PRECHARGE_RELAY     9   // Relay Pre-Charge 72V
#define PIN_THROTTLE_OUT        10  // PWM / DAC Output ke Inverter Motor
#define PIN_REGEN_BRAKE_OUT     11  // Sinyal E-Braking ke Inverter
#define PIN_REVERSE_DIR_OUT     12  // Sinyal Arah Mundur ke Inverter

// Bus Komunikasi CAN (TWAI ESP32)
#define PIN_CAN_TX              17  // CAN Bus Transceiver TX
#define PIN_CAN_RX              18  // CAN Bus Transceiver RX

// Komunikasi UART Modul Seluler 4G LTE
#define PIN_LTE_TX              43
#define PIN_LTE_RX              44

// Komunikasi I2C Display / Sensor Tambahan
#define PIN_I2C_SDA             38
#define PIN_I2C_SCL             39

// ==========================================
// PARAMETER KALIBRASI & THRESHOLD AMAN
// ==========================================
#define APPS_VOLT_MIN_VALID     0.50f   // Batas bawah voltase ADC valid
#define APPS_VOLT_MAX_VALID     4.50f   // Batas atas voltase ADC valid
#define APPS_DEADBAND_PERCENT   3.0f    // Deadband gas awal 3%
#define APPS_MAX_DISCREPANCY    10.0f   // Toleransi selisih APPS1 & APPS2 (%)
#define APPS_FAULT_TIMEOUT_MS   100     // Waktu toleransi mismatch sebelum cut-off

#define PRECHARGE_DURATION_MS   1500    // Durasi pre-charge sebelum kontaktor ON
#define MOTOR_TEMP_WARN_C       75.0f   // Suhu peringatan motor
#define MOTOR_TEMP_CUTOFF_C     95.0f   // Suhu pemutusan darurat motor
#define INVERTER_TEMP_CUTOFF_C  85.0f   // Suhu pemutusan darurat inverter
#define BATT_MIN_VOLTAGE        60.0f   // Tegangan baterai minimum (Pack 72V)
#define BATT_MAX_VOLTAGE        84.0f   // Tegangan baterai maksimum full charge

// ==========================================
// KONFIGURASI TELEMETRI & JARINGAN
// ==========================================
#define MQTT_BROKER_HOST        "broker.emqx.io"
#define MQTT_BROKER_PORT        1883
#define MQTT_TELEMETRY_INTERVAL 500     // Kirim data setiap 500 ms (2 Hz)

#endif // CONFIG_H
