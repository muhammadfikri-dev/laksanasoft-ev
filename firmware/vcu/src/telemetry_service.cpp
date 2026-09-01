#include "telemetry_service.h"

TelemetryService::TelemetryService() : 
    _mqttClient(_wifiClient), 
    _lastPublishTimeMs(0) {}

void TelemetryService::init() {
    // Inisialisasi MQTT Broker
    _mqttClient.setServer(MQTT_BROKER_HOST, MQTT_BROKER_PORT);
    _mqttClient.setBufferSize(1024);
    
    // Inisialisasi WiFi (atau fallback jika mode offline)
    WiFi.mode(WIFI_STA);
    WiFi.begin("LAKSANASOFT-AP", "LaksanaEV2026");
}

void TelemetryService::reconnectMqtt() {
    if (!_mqttClient.connected() && WiFi.status() == WL_CONNECTED) {
        String clientId = "VCU-" + String(VEHICLE_VIN);
        _mqttClient.connect(clientId.c_str());
    }
}

String TelemetryService::createJsonPayload(const VehicleTelemetry& telemetry) {
    StaticJsonDocument<768> doc;
    
    doc["vin"] = VEHICLE_VIN;
    doc["timestamp"] = millis() / 1000;
    
    JsonObject sys = doc.createNestedObject("system");
    sys["state"] = getVehicleStateString(telemetry.currentState);
    sys["mode"] = getDriveModeString(telemetry.currentMode);
    sys["ready"] = telemetry.isReady;
    sys["side_stand"] = telemetry.isSideStandDown;
    sys["brake"] = telemetry.isBrakeEngaged;
    
    JsonObject dyn = doc.createNestedObject("dynamics");
    dyn["speed_kmh"] = round(telemetry.vehicleSpeedKmh * 10.0f) / 10.0f;
    dyn["motor_rpm"] = telemetry.motorRpm;
    dyn["throttle_pct"] = round(telemetry.throttlePercent * 10.0f) / 10.0f;
    
    JsonObject pwr = doc.createNestedObject("energy");
    pwr["pack_voltage"] = round(telemetry.packVoltage * 10.0f) / 10.0f;
    pwr["pack_current"] = round(telemetry.packCurrentAmp * 10.0f) / 10.0f;
    pwr["power_watts"] = round(telemetry.packPowerWatts);
    pwr["soc"] = telemetry.batterySocPercent;
    pwr["soh"] = telemetry.batterySohPercent;
    pwr["range_km"] = round(telemetry.estimatedRangeKm * 10.0f) / 10.0f;
    
    JsonObject temp = doc.createNestedObject("temperatures");
    temp["motor_c"] = round(telemetry.motorTempC * 10.0f) / 10.0f;
    temp["inverter_c"] = round(telemetry.inverterTempC * 10.0f) / 10.0f;
    temp["bms_c"] = round(telemetry.bmsMaxTempC * 10.0f) / 10.0f;
    
    JsonObject diag = doc.createNestedObject("diagnostics");
    diag["fault_code"] = (int)telemetry.activeFault;
    diag["fault_msg"] = getFaultCodeString(telemetry.activeFault);
    
    String output;
    serializeJson(doc, output);
    return output;
}

void TelemetryService::process(const VehicleTelemetry& telemetry) {
    uint32_t now = millis();
    if (now - _lastPublishTimeMs >= MQTT_TELEMETRY_INTERVAL) {
        _lastPublishTimeMs = now;
        
        if (WiFi.status() == WL_CONNECTED) {
            if (!_mqttClient.connected()) {
                reconnectMqtt();
            }
            if (_mqttClient.connected()) {
                String payload = createJsonPayload(telemetry);
                String topic = "laksanasoft/ev/v1/" + String(VEHICLE_VIN) + "/telemetry";
                _mqttClient.publish(topic.c_str(), payload.c_str());
            }
        }
        
        // Log ke Serial Console untuk debugging lokal
        // String debugJson = createJsonPayload(telemetry);
        // Serial.println(debugJson);
    }
}
