#include "can_manager.h"

CanManager::CanManager() : _isInitialized(false), _lastBroadcastTimeMs(0) {}

bool CanManager::init() {
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT((gpio_num_t)PIN_CAN_TX, (gpio_num_t)PIN_CAN_RX, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_250KBITS(); // 250 kbps standar otomotif
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
        if (twai_start() == ESP_OK) {
            _isInitialized = true;
            return true;
        }
    }
    _isInitialized = false;
    return false;
}

void CanManager::broadcastStatus(const VehicleTelemetry& telemetry) {
    if (!_isInitialized) return;
    
    uint32_t now = millis();
    if (now - _lastBroadcastTimeMs < 50) return; // 20 Hz
    _lastBroadcastTimeMs = now;

    // Frame 0x180: Perintah Kontrol VCU -> Inverter
    twai_message_t cmdMsg;
    cmdMsg.identifier = 0x180;
    cmdMsg.extd = 0;
    cmdMsg.data_length_code = 8;
    
    uint16_t torqueWord = (uint16_t)(telemetry.targetTorquePercent * 10.0f);
    cmdMsg.data[0] = (uint8_t)(torqueWord >> 8);
    cmdMsg.data[1] = (uint8_t)(torqueWord & 0xFF);
    cmdMsg.data[2] = telemetry.isReady ? 0x01 : 0x00;
    if (telemetry.isReverseActive) cmdMsg.data[2] |= 0x02;
    if (telemetry.currentState == STATE_REGEN) cmdMsg.data[2] |= 0x04;
    cmdMsg.data[3] = 100; // Limit arus 100A
    cmdMsg.data[4] = 0;
    cmdMsg.data[5] = 0;
    cmdMsg.data[6] = 0;
    cmdMsg.data[7] = (uint8_t)(telemetry.activeFault);

    twai_transmit(&cmdMsg, pdMS_TO_TICKS(5));

    // Frame 0x480: Data Dashboard HMI
    twai_message_t dashMsg;
    dashMsg.identifier = 0x480;
    dashMsg.extd = 0;
    dashMsg.data_length_code = 8;
    
    dashMsg.data[0] = (uint8_t)telemetry.vehicleSpeedKmh;
    dashMsg.data[1] = (uint8_t)telemetry.currentMode;
    dashMsg.data[2] = telemetry.batterySocPercent;
    dashMsg.data[3] = (telemetry.isSideStandDown ? 0x01 : 0x00) | (telemetry.isBrakeEngaged ? 0x02 : 0x00);
    uint16_t estRange = (uint16_t)(telemetry.estimatedRangeKm * 10.0f);
    dashMsg.data[4] = (uint8_t)(estRange >> 8);
    dashMsg.data[5] = (uint8_t)(estRange & 0xFF);
    dashMsg.data[6] = (uint8_t)telemetry.motorTempC;
    dashMsg.data[7] = (uint8_t)telemetry.inverterTempC;

    twai_transmit(&dashMsg, pdMS_TO_TICKS(5));
}

void CanManager::checkIncomingMessages(VehicleTelemetry& telemetry) {
    if (!_isInitialized) return;

    twai_message_t rxMsg;
    while (twai_receive(&rxMsg, 0) == ESP_OK) {
        if (rxMsg.identifier == 0x280) { // Feedback Inverter
            int16_t rpm = (rxMsg.data[0] << 8) | rxMsg.data[1];
            telemetry.motorRpm = rpm;
            telemetry.inverterTempC = (float)(rxMsg.data[4] - 40);
            telemetry.motorTempC = (float)(rxMsg.data[5] - 40);
        }
    }
}
