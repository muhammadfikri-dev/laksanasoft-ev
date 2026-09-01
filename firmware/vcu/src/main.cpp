/**
 * ============================================================================
 * LAKSANASOFT-EV: VEHICLE CONTROL UNIT (VCU) MAIN FIRMWARE
 * Platform: ESP32-S3 Dual Core @ 240MHz (FreeRTOS)
 * Arsitektur: Dual-Core Task Separation (Safety-Critical & Telemetry)
 * Hak Cipta (c) 2026 PT. Laksana Digital Industri
 * ============================================================================
 */

#include <Arduino.h>
#include "config.h"
#include "vcu_state.h"
#include "throttle_handler.h"
#include "bms_driver.h"
#include "motor_controller.h"
#include "can_manager.h"
#include "telemetry_service.h"

// Objek Sub-Sistem
ThrottleHandler throttleHandler;
BmsDriver bmsDriver;
MotorController motorController;
CanManager canManager;
TelemetryService telemetryService;

// State & Telemetri Global Kendaraan
VehicleTelemetry g_telemetry;
portMUX_TYPE g_telemetryMutex = portMUX_INITIALIZER_UNLOCKED;

// Waktu transisi status
uint32_t g_prechargeStartTimeMs = 0;
uint32_t g_lastModeBtnPressMs = 0;

// Task Handles FreeRTOS
TaskHandle_t hControlTask;
TaskHandle_t hTelemetryTask;

// ============================================================================
// HARDWARE INITIALIZATION & INPUT READING
// ============================================================================
void initPins() {
    pinMode(PIN_BRAKE_INPUT, INPUT_PULLUP);
    pinMode(PIN_SIDE_STAND, INPUT_PULLUP);
    pinMode(PIN_MODE_BUTTON, INPUT_PULLUP);
    pinMode(PIN_REVERSE_SWITCH, INPUT_PULLUP);

    pinMode(PIN_MAIN_CONTACTOR, OUTPUT);
    pinMode(PIN_PRECHARGE_RELAY, OUTPUT);

    digitalWrite(PIN_MAIN_CONTACTOR, LOW);
    digitalWrite(PIN_PRECHARGE_RELAY, LOW);
}

void readDigitalSensors(VehicleTelemetry& tel) {
    // Input Active-LOW dengan pullup internal
    tel.isBrakeEngaged = (digitalRead(PIN_BRAKE_INPUT) == LOW);
    tel.isSideStandDown = (digitalRead(PIN_SIDE_STAND) == LOW);
    tel.isReverseActive = (digitalRead(PIN_REVERSE_SWITCH) == LOW);

    // Pergantian mode berkendara (Toggle Button Debounce 300ms)
    if (digitalRead(PIN_MODE_BUTTON) == LOW && (millis() - g_lastModeBtnPressMs > 300)) {
        g_lastModeBtnPressMs = millis();
        if (tel.currentMode == MODE_ECO) tel.currentMode = MODE_URBAN;
        else if (tel.currentMode == MODE_URBAN) tel.currentMode = MODE_SPORT;
        else if (tel.currentMode == MODE_SPORT) tel.currentMode = MODE_ECO;
    }

    if (tel.isReverseActive) {
        tel.currentMode = MODE_REVERSE;
    }
}

// ============================================================================
// CORE 0 TASK: SAFETY-CRITICAL VEHICLE CONTROL LOOP (100 Hz / 10 ms)
// ============================================================================
void ControlLoopTask(void* pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(10); // 10ms = 100 Hz

    for (;;) {
        portENTER_CRITICAL(&g_telemetryMutex);
        VehicleTelemetry localTel = g_telemetry;
        portEXIT_CRITICAL(&g_telemetryMutex);

        readDigitalSensors(localTel);

        // 1. Baca & Validasi Throttle APPS
        FaultCode throttleFault = FAULT_NONE;
        float rawThrottle = 0.0f;
        bool appsValid = throttleHandler.process(rawThrottle, localTel.apps1Voltage, localTel.apps2Voltage, throttleFault);

        // 2. Mesin Status Utama Kendaraan (State Machine)
        switch (localTel.currentState) {
            case STATE_BOOT:
                digitalWrite(PIN_MAIN_CONTACTOR, LOW);
                digitalWrite(PIN_PRECHARGE_RELAY, LOW);
                motorController.emergencyStop();
                
                // Mulai Precharge jika sensor awal dalam batas normal
                localTel.currentState = STATE_PRECHARGE;
                g_prechargeStartTimeMs = millis();
                digitalWrite(PIN_PRECHARGE_RELAY, HIGH);
                break;

            case STATE_PRECHARGE:
                // Menunggu kapasitor terisi selama 1.5 detik
                if (millis() - g_prechargeStartTimeMs >= PRECHARGE_DURATION_MS) {
                    digitalWrite(PIN_MAIN_CONTACTOR, HIGH); // Hubungkan kontak utama
                    vTaskDelay(pdMS_TO_TICKS(50));
                    digitalWrite(PIN_PRECHARGE_RELAY, LOW);  // Matikan relay pre-charge
                    localTel.currentState = STATE_STANDBY;
                }
                break;

            case STATE_STANDBY:
                motorController.emergencyStop();
                localTel.isReady = false;
                localTel.targetTorquePercent = 0.0f;

                // Syarat Masuk ke STATE_DRIVE (Arming System):
                // Standar samping naik, tuas rem ditekan, dan gas idle 0%
                if (!localTel.isSideStandDown && localTel.isBrakeEngaged && rawThrottle < 1.0f) {
                    localTel.isReady = true;
                    localTel.currentState = STATE_DRIVE;
                }
                break;

            case STATE_DRIVE:
                // Pengecekan Interlock Standar Samping
                if (localTel.isSideStandDown) {
                    localTel.currentState = STATE_STANDBY;
                    break;
                }

                // Pengecekan Interlock Pengereman (BOA / E-Brake)
                if (localTel.isBrakeEngaged) {
                    if (localTel.vehicleSpeedKmh > 5.0f && localTel.batterySocPercent < 95) {
                        localTel.currentState = STATE_REGEN;
                    } else {
                        motorController.setTorqueDemand(0.0f, false);
                    }
                    break;
                }

                // Pengecekan Validitas Sinyal Gas (APPS Failsafe)
                if (!appsValid) {
                    localTel.activeFault = throttleFault;
                    localTel.currentState = STATE_FAULT;
                    break;
                }

                // Hitung Permintaan Torsi Sesuai Mode
                localTel.throttlePercent = rawThrottle;
                localTel.targetTorquePercent = throttleHandler.mapTorqueToMode(rawThrottle, localTel.currentMode);

                // Thermal Derating Protection
                if (localTel.motorTempC >= MOTOR_TEMP_WARN_C) {
                    float derateFactor = 1.0f - ((localTel.motorTempC - MOTOR_TEMP_WARN_C) / (MOTOR_TEMP_CUTOFF_C - MOTOR_TEMP_WARN_C));
                    derateFactor = constrain(derateFactor, 0.2f, 1.0f);
                    localTel.targetTorquePercent *= derateFactor;
                }

                if (localTel.motorTempC >= MOTOR_TEMP_CUTOFF_C || localTel.inverterTempC >= INVERTER_TEMP_CUTOFF_C) {
                    localTel.activeFault = FAULT_OVER_TEMPERATURE;
                    localTel.currentState = STATE_FAULT;
                    break;
                }

                motorController.setTorqueDemand(localTel.targetTorquePercent, localTel.isReverseActive);
                break;

            case STATE_REGEN:
                // Pengereman regeneratif aktif
                if (!localTel.isBrakeEngaged || localTel.vehicleSpeedKmh < 3.0f || localTel.batterySocPercent >= 96) {
                    motorController.setRegenDemand(0.0f);
                    localTel.currentState = STATE_DRIVE;
                } else {
                    motorController.setRegenDemand(50.0f); // 50% kekuatan pengereman listrik
                }
                break;

            case STATE_FAULT:
                motorController.emergencyStop();
                localTel.isReady = false;
                // Pemulihan otomatis jika kondisi fault telah teratasi dan gas dilepas penuh
                if (rawThrottle < 1.0f && appsValid && !localTel.isSideStandDown) {
                    localTel.activeFault = FAULT_NONE;
                    localTel.currentState = STATE_STANDBY;
                }
                break;

            case STATE_SHUTDOWN:
                motorController.emergencyStop();
                digitalWrite(PIN_MAIN_CONTACTOR, LOW);
                digitalWrite(PIN_PRECHARGE_RELAY, LOW);
                localTel.isReady = false;
                break;

            default:
                localTel.currentState = STATE_BOOT;
                break;
        }

        motorController.updateFeedback(localTel);

        portENTER_CRITICAL(&g_telemetryMutex);
        g_telemetry = localTel;
        portEXIT_CRITICAL(&g_telemetryMutex);

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

// ============================================================================
// CORE 1 TASK: TELEMETRY, BMS, CAN & IOT GATEWAY (10 Hz / 100 ms)
// ============================================================================
void TelemetryLoopTask(void* pvParameters) {
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(100); // 100ms = 10 Hz

    for (;;) {
        portENTER_CRITICAL(&g_telemetryMutex);
        VehicleTelemetry snapTel = g_telemetry;
        portEXIT_CRITICAL(&g_telemetryMutex);

        // 1. Baca Data Paket Baterai dari Smart BMS
        bmsDriver.update(snapTel);

        // 2. Periksa Pesan CAN Bus & Broadcast Telemetri
        canManager.checkIncomingMessages(snapTel);
        canManager.broadcastStatus(snapTel);

        // 3. Estimasi Sisa Jarak Tempuh (Range Estimation)
        // Konsumsi rata-rata komuter: ~30 Wh/km. Baterai 3240 Wh total
        float remainingEnergyWh = (snapTel.batterySocPercent / 100.0f) * 3240.0f;
        snapTel.estimatedRangeKm = remainingEnergyWh / 30.0f;

        snapTel.uptimeSeconds = millis() / 1000;

        // 4. Publikasi Data ke Cloud MQTT / Wi-Fi
        telemetryService.process(snapTel);

        portENTER_CRITICAL(&g_telemetryMutex);
        g_telemetry = snapTel;
        portEXIT_CRITICAL(&g_telemetryMutex);

        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

// ============================================================================
// SETUP & MAIN ARDUINO ENTRY
// ============================================================================
void setup() {
    Serial.begin(115200);
    delay(500);

    Serial.println("=================================================");
    Serial.println("  LAKSANASOFT-EV VCU FIRMWARE v1.0.0 INITIALIZING ");
    Serial.println("  (c) 2026 PT. Laksana Digital Industri           ");
    Serial.println("=================================================");

    initPins();
    throttleHandler.init();
    bmsDriver.init();
    motorController.init();
    canManager.init();
    telemetryService.init();

    // Inisialisasi struktur telemetri awal
    memset(&g_telemetry, 0, sizeof(VehicleTelemetry));
    g_telemetry.currentState = STATE_BOOT;
    g_telemetry.currentMode = MODE_URBAN;
    g_telemetry.batterySocPercent = 100;
    g_telemetry.batterySohPercent = 100;
    g_telemetry.packVoltage = 74.2f;

    // Menjalankan Control Loop pada Core 0 (Priority 5 - Highest)
    xTaskCreatePinnedToCore(
        ControlLoopTask,
        "ControlTask",
        4096,
        NULL,
        5,
        &hControlTask,
        0
    );

    // Menjalankan Telemetry & IoT pada Core 1 (Priority 2)
    xTaskCreatePinnedToCore(
        TelemetryLoopTask,
        "TelemetryTask",
        8192,
        NULL,
        2,
        &hTelemetryTask,
        1
    );

    Serial.println("[VCU] Sistem Dual-Core FreeRTOS berhasil diaktifkan.");
}

void loop() {
    // Loop kosong karena seluruh eksekusi dikelola oleh Task FreeRTOS
    vTaskDelay(pdMS_TO_TICKS(1000));
}
