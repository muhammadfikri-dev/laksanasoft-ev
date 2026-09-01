# 📡 Protokol Komunikasi CAN Bus & Telemetri MQTT (Laksanasoft-EV)

Dokumen ini mendefinisikan standar komunikasi internal kendaraan menggunakan bus data otomotif **CAN 2.0B (Controller Area Network)** dan komunikasi awan (*cloud*) berbasis **MQTT & WebSocket**.

---

## 1. Arsitektur Bus Data Kendaraan (CAN Bus 2.0B)

- **Baud Rate:** 250 kbps (Standar Otomotif SAE J1939) atau 500 kbps
- **Struktur ID:** 11-bit Standard ID & 29-bit Extended Identifier
- **Terminasi Bus:** Resistor $120\,\Omega$ di kedua ujung bus fisik (VCU & Motor Controller).

### Matriks Pesan CAN Bus:

| CAN ID (Hex) | Pengirim | Penerima | Interval | Deskripsi Sinyal & Format Data |
| :--- | :--- | :--- | :--- | :--- |
| **0x180** | VCU | Inverter | 20 ms | **VCU Control Command:**<br>Byte 0-1: Torque Target (0-1000 = 0-100.0%)<br>Byte 2: Control Word (Bit 0: Enable, Bit 1: Reverse, Bit 2: Regen Active)<br>Byte 3: Max Discharge Current Limit (A)<br>Byte 4-7: Reserved / Checksum |
| **0x280** | Inverter | VCU | 20 ms | **Inverter Status & Feedback:**<br>Byte 0-1: Actual Motor RPM (signed int16)<br>Byte 2-3: Phase Current RMS (0.1A/bit)<br>Byte 4: Inverter Temp (°C, offset -40)<br>Byte 5: Motor Temp (°C, offset -40)<br>Byte 6-7: Inverter Fault Bitfield |
| **0x380** | Smart BMS | VCU | 100 ms | **BMS Pack Telemetry:**<br>Byte 0-1: Total Pack Voltage (0.1V/bit, e.g. 720 = 72.0V)<br>Byte 2-3: Pack Current (0.1A/bit, signed)<br>Byte 4: State of Charge / SoC (0 - 100%)<br>Byte 5: State of Health / SoH (0 - 100%)<br>Byte 6: Highest Cell Temp (°C)<br>Byte 7: BMS Status / Alarm Flags |
| **0x480** | VCU | HMI Cluster | 50 ms | **HMI Dashboard Display Stream:**<br>Byte 0: Speed (km/h)<br>Byte 1: Drive Mode (0=ECO, 1=URBAN, 2=SPORT, 3=REVERSE)<br>Byte 2: Battery SoC (%)<br>Byte 3: State Flags (Ready, SideStand, Brake, Headlight, Fault)<br>Byte 4-5: Estimated Range Remaining (km)<br>Byte 6-7: Power Usage (0.01 kW/bit) |

---

## 2. Protokol Telemetri Cloud (MQTT Gateway)

VCU mengirimkan data berkala ke broker MQTT (lokal atau cloud) melalui konektivitas Wi-Fi atau modul seluler 4G LTE SIMCOM A7670E.

### Struktur Topik MQTT:
- **Publish Telemetri:** `laksanasoft/ev/v1/{VIN}/telemetry` (Frekuensi: 2 Hz saat melaju, 0.1 Hz saat standby)
- **Publish Peringatan Bahaya (Alerts):** `laksanasoft/ev/v1/{VIN}/alerts` (QoS 2, seketika saat fault)
- **Subscribe Perintah Kontrol Jarak Jauh:** `laksanasoft/ev/v1/{VIN}/commands` (e.g. Remote Lock, Immobolizer, Horn Finder)
- **Publish Respon Perintah:** `laksanasoft/ev/v1/{VIN}/responses`

### Format Payload Telemetri JSON (`telemetry`):

```json
{
  "vin": "LAKSANAEV2026ID001",
  "timestamp": 1788242400,
  "system": {
    "state": "STATE_DRIVE",
    "mode": "SPORT",
    "uptime_sec": 3420,
    "ready": true,
    "side_stand": false,
    "brake_active": false
  },
  "dynamics": {
    "speed_kmh": 62.4,
    "motor_rpm": 3240,
    "odometer_km": 1420.8,
    "trip_km": 18.4
  },
  "energy": {
    "pack_voltage": 74.2,
    "pack_current_amp": 38.6,
    "power_watts": 2864.1,
    "soc_percent": 82,
    "soh_percent": 98,
    "energy_consumed_wh": 460.2,
    "efficiency_wh_per_km": 25.01,
    "range_est_km": 76.5
  },
  "temperatures": {
    "motor_temp_c": 58.0,
    "inverter_temp_c": 46.5,
    "battery_max_temp_c": 34.0,
    "ambient_temp_c": 29.5
  },
  "diagnostics": {
    "fault_code": 0,
    "fault_message": "NORMAL",
    "can_bus_ok": true,
    "cell_imbalance_mv": 12
  },
  "location": {
    "lat": -3.795420,
    "lng": 114.778150,
    "altitude_m": 24.0,
    "heading_deg": 142.0,
    "satellites": 9
  }
}
```

---

## 3. Integrasi WebSocket Real-Time untuk Digital Cluster HMI

Backend `telemetry-server` menyediakan antarmuka WebSocket `ws://<server_ip>:5000/ws/telemetry` untuk memperbarui antarmuka instrumen digital (dashboard speedometer) dengan latensi rendah ($< 30\text{ ms}$).
