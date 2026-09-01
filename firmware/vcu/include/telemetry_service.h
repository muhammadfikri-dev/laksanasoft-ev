#ifndef TELEMETRY_SERVICE_H
#define TELEMETRY_SERVICE_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "config.h"
#include "vcu_state.h"

class TelemetryService {
public:
    TelemetryService();
    void init();
    void process(const VehicleTelemetry& telemetry);
    String createJsonPayload(const VehicleTelemetry& telemetry);

private:
    WiFiClient _wifiClient;
    PubSubClient _mqttClient;
    uint32_t _lastPublishTimeMs;
    
    void reconnectMqtt();
};

#endif // TELEMETRY_SERVICE_H
