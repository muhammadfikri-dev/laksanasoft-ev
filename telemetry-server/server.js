/**
 * ============================================================================
 * LAKSANASOFT-EV TELEMETRY BACKEND SERVER
 * Express API + WebSocket Streamer + MQTT Gateway
 * (c) 2026 PT. Laksana Digital Industri
 * ============================================================================
 */

require('dotenv').config();
const http = require('http');
const express = require('express');
const cors = require('cors');
const { WebSocketServer, WebSocket } = require('ws');
const mqtt = require('mqtt');

const PORT = process.env.PORT || 5000;
const MQTT_BROKER = process.env.MQTT_BROKER || 'mqtt://broker.emqx.io:1883';
const MQTT_TOPIC_TELEMETRY = 'laksanasoft/ev/v1/+/telemetry';
const MQTT_TOPIC_ALERTS = 'laksanasoft/ev/v1/+/alerts';

const app = express();
app.use(cors());
app.use(express.json());

// In-Memory Cache Telemetri Terakhir per Kendaraan
const vehicleStore = new Map();

// Default Telemetri Mock jika baru start
const defaultTelemetry = {
  vin: "LAKSANAEV2026ID001",
  timestamp: Math.floor(Date.now() / 1000),
  system: {
    state: "STATE_DRIVE",
    mode: "SPORT",
    ready: true,
    side_stand: false,
    brake: false
  },
  dynamics: {
    speed_kmh: 0.0,
    motor_rpm: 0,
    throttle_pct: 0.0
  },
  energy: {
    pack_voltage: 74.2,
    pack_current: 0.0,
    power_watts: 0,
    soc: 88,
    soh: 99,
    range_km: 84.5
  },
  temperatures: {
    motor_c: 42.0,
    inverter_c: 38.5,
    bms_c: 32.0
  },
  diagnostics: {
    fault_code: 0,
    fault_msg: "NORMAL"
  }
};
vehicleStore.set("LAKSANAEV2026ID001", defaultTelemetry);

// ============================================================================
// REST API ENDPOINTS
// ============================================================================

app.get('/api/v1/health', (req, res) => {
  res.json({
    status: 'ONLINE',
    service: 'Laksanasoft-EV Telemetry Hub',
    timestamp: new Date().toISOString(),
    connected_vehicles: vehicleStore.size,
    ws_clients: wss.clients.size
  });
});

app.get('/api/v1/telemetry/latest', (req, res) => {
  const vin = req.query.vin || "LAKSANAEV2026ID001";
  const data = vehicleStore.get(vin) || defaultTelemetry;
  res.json(data);
});

app.post('/api/v1/telemetry/ingest', (req, res) => {
  const payload = req.body;
  if (!payload || !payload.vin) {
    return res.status(400).json({ error: 'Payload harus menyertakan VIN kendaraan' });
  }

  vehicleStore.set(payload.vin, payload);
  broadcastWebSocket(payload);
  res.json({ success: true, received_at: Date.now() });
});

app.post('/api/v1/command', (req, res) => {
  const { vin, command, params } = req.body;
  if (!vin || !command) {
    return res.status(400).json({ error: 'VIN dan command harus diisi' });
  }

  const topic = `laksanasoft/ev/v1/${vin}/commands`;
  const msg = JSON.stringify({ command, params, timestamp: Date.now() });
  
  if (mqttClient && mqttClient.connected) {
    mqttClient.publish(topic, msg, { qos: 1 });
    res.json({ success: true, message: `Perintah [${command}] berhasil dikirim ke ${vin}` });
  } else {
    res.status(503).json({ error: 'MQTT Broker tidak terhubung' });
  }
});

// ============================================================================
// HTTP & WEBSOCKET SERVER
// ============================================================================

const server = http.createServer(app);
const wss = new WebSocketServer({ server, path: '/ws/telemetry' });

function broadcastWebSocket(data) {
  const msg = JSON.stringify(data);
  wss.clients.forEach(client => {
    if (client.readyState === WebSocket.OPEN) {
      client.send(msg);
    }
  });
}

wss.on('connection', (ws) => {
  console.log(`[WebSocket] Client HMI/Dashboard terhubung. Total client: ${wss.clients.size}`);
  
  // Kirim data terakhir saat koneksi baru terbentuk
  const latest = vehicleStore.get("LAKSANAEV2026ID001") || defaultTelemetry;
  ws.send(JSON.stringify(latest));

  ws.on('close', () => {
    console.log(`[WebSocket] Client terputus. Sisa client: ${wss.clients.size}`);
  });
});

// ============================================================================
// MQTT CLIENT GATEWAY
// ============================================================================

console.log(`[MQTT] Menghubungkan ke broker: ${MQTT_BROKER}`);
const mqttClient = mqtt.connect(MQTT_BROKER, {
  reconnectPeriod: 3000,
  connectTimeout: 10000
});

mqttClient.on('connect', () => {
  console.log('[MQTT] Berhasil terhubung ke Broker Telemetri!');
  mqttClient.subscribe([MQTT_TOPIC_TELEMETRY, MQTT_TOPIC_ALERTS], (err) => {
    if (!err) {
      console.log(`[MQTT] Subscribed ke topik: ${MQTT_TOPIC_TELEMETRY}`);
    }
  });
});

mqttClient.on('message', (topic, message) => {
  try {
    const payload = JSON.parse(message.toString());
    if (payload.vin) {
      vehicleStore.set(payload.vin, payload);
      broadcastWebSocket(payload);
    }
  } catch (err) {
    console.error('[MQTT] Gagal parse payload JSON:', err.message);
  }
});

mqttClient.on('error', (err) => {
  console.warn('[MQTT] Warning:', err.message);
});

// Start Server
server.listen(PORT, () => {
  console.log(`========================================================`);
  console.log(` ⚡ LAKSANASOFT-EV TELEMETRY SERVER AKTIF`);
  console.log(` 🚀 REST API     : http://localhost:${PORT}/api/v1/health`);
  console.log(` 📡 WebSocket   : ws://localhost:${PORT}/ws/telemetry`);
  console.log(`========================================================`);
});
