# ⚡ Laksanasoft-EV: Platform Sepeda Motor Listrik Modular & Cerdas (120 km/jam High Performance)

[![Platform](https://img.shields.io/badge/Platform-ESP32--S3%20%7C%20FreeRTOS-blue.svg)](https://www.espressif.com/)
[![CAN Bus](https://img.shields.io/badge/Protocol-CAN%202.0B%20%7C%20MQTT-brightgreen.svg)]()
[![License](https://img.shields.io/badge/License-MIT-orange.svg)](LICENSE)
[![Status](https://img.shields.io/badge/Status-120%20km%2Fh%20Ready-success.svg)]()

**Laksanasoft-EV** adalah rancang bangun ekosistem terbuka (*open-architecture*) untuk konversi dan manufaktur sepeda motor listrik berperforma tinggi yang mampu mencapai kecepatan puncak **120 km/jam** secara stabil, aman, mudah dirakit, dan didukung komponen yang **sangat mudah dijangkau di pasaran Indonesia**.

Platform ini mencakup master wiring diagram pin-ke-pin, kalkulasi daya traksi 120 km/jam, Vehicle Control Unit (VCU) ESP32-S3 berbasis FreeRTOS, integrasi Inverter FOC dengan fitur *Flux Weakening*, Battery Management System (BMS) pintar, instrumentasi digital (HMI 160 km/h scale), serta backend telemetri *real-time*.

---

## 📌 Daftar Isi
1. [Spesifikasi Edisi 120 km/jam](#-spesifikasi-edisi-120-kmjam)
2. [Alat & Bahan yang Mudah Dijangkau](#-alat--bahan-yang-mudah-dijangkau-pasar-indonesia)
3. [Master Wiring Diagram Ringkas](#-master-wiring-diagram-ringkas)
4. [Struktur Repositori](#-struktur-repositori)
5. [Dokumentasi Lengkap](#-dokumentasi-lengkap)
6. [Panduan Memulai Cepat (Quick Start)](#-panduan-memulai-cepat-quick-start)
7. [Lisensi](#-lisensi)

---

## ⚡ Spesifikasi Edisi 120 km/jam

| Parameter | Spesifikasi High-Performance (120 km/jam) |
| :--- | :--- |
| **Kecepatan Maksimal** | **120 – 125 km/jam** (Stabil dengan Active Flux Weakening) |
| **Akselerasi (0 - 100 km/jam)**| **< 5.5 Detik** |
| **Tipe Motor Traksi** | **QS Motor 138 90H V3 Mid-Drive** (Daya Peak 15kW – 18kW, Torsi 72Nm) |
| **Rasio Reduksi Gir** | **1 : 3.0** (Sprocket 15T : 45T Rantai Heavy Duty 428H) |
| **Inverter / Controller** | **FarDriver ND72680** atau **Votol EM-200-2 / EM-260** (200A Bus / 550A-680A Phase) |
| **Tegangan & Kapasitas Baterai**| **72V 50Ah – 60Ah (3.6 – 4.3 kWh)** High Discharge ($\ge 3\text{C}-5\text{C}$) |
| **Smart BMS** | **Smart BMS 20S 200A – 250A Peak** dengan Active Balancing 2A |
| **Proteksi Arus Tinggi (HV)**| **MCB DC 2-Pole 150A – 200A** + XT90-S Anti-Spark |
| **Kabel Daya Utama** | **Kabel Silikon Tahan Panas 200°C AWG 4 / AWG 2 (25 – 35 mm²)** |
| **Sistem Pengereman** | **Double Disc Brake 4-Piston Depan + Regenerative Braking** |
| **Estimasi Jarak Tempuh** | **80 – 110 km** (Mode ECO/URBAN) / **60 – 75 km** (High-Speed Cruising) |

---

## 🛒 Alat & Bahan yang Mudah Dijangkau (Pasar Indonesia)

Semua komponen dirancang agar mudah dibeli di **Tokopedia, Shopee, Toko Listrik, dan Toko Variasi Motor**:
- **Motor & Controller 120 km/h:** QS Motor 138 90H V3 + FarDriver ND72680 / Votol EM-200-2 (kabel PNP).
- **Pengaman:** MCB DC 2-Pole 150A-200A (Merk Nader/Chint/FEEO - toko listrik / PLTS).
- **Stepdown 12V:** Modul DC-DC 72V ke 12V 20A Alumunium (240W).
- **Kelistrikan 12V:** Kunci kontak Beat/Vario, Relay klakson Bosch/Denso, Sekring blade 10A/15A.
- **Kabel:** Kabel Silikon AWG 4 / Kabel NYAF 25 mm² tembaga murni SNI.

👉 Pelajari selengkapnya di [**Panduan Belanja Lokal Indonesia**](docs/02b-panduan-belanja-lokal-indonesia.md).

---

## 🔌 Master Wiring Diagram Ringkas

```text
[ BATERAI 72V (+) ] ---> [ MCB DC 150A/200A ] ---> [ XT90-S ] ---> [ Controller B+ ]
[ BATERAI 72V (-) ] ---> [ BMS P- ] -----------------------------> [ Controller B- ]

[ KUNCI KONTAK ] ======> Memberi daya 72V ke:
                         ├── 1. Kabel Oranye Controller (Electric Lock / Ignition)
                         └── 2. Input DC-DC Stepdown (72V ke 12V 20A)

[ OUTPUT DC-DC 12V ] ==> [ Fuse Box 12V ] ===> Menyuplai Lampu LED, Sein, Klakson, & Switch Rem
```

👉 Skema lengkap pin-to-pin, kode warna kabel, dan terminal relay tersedia di [**Skematik & Wiring Diagram Lengkap**](docs/03-skematik-dan-wiring-diagram.md).

---

## 📂 Struktur Repositori

```text
laksanasoft-ev/
├── docs/                                     # Dokumentasi teknis komprehensif
│   ├── 01-spesifikasi-sistem.md              # Kalkulasi traksi 120 km/jam, rasio gir & daya
│   ├── 02-bill-of-materials-bom.md           # BOM part standar & upgrade 120 km/jam
│   ├── 02b-panduan-belanja-lokal-indonesia.md# Panduan belanja & link part e-commerce lokal
│   ├── 03-skematik-dan-wiring-diagram.md     # Master wiring diagram, warna kabel & pinout
│   ├── 04-alur-dan-cara-kerja.md             # State machine, alur start-up & Flux Weakening
│   ├── 05-protokol-keamanan-failsafe.md      # Standar keselamatan & penanganan fault
│   └── 06-telemetri-dan-iot.md               # Spesifikasi frame CAN & MQTT payload
├── firmware/vcu/                             # Source code Vehicle Control Unit (ESP32-S3)
│   ├── include/                              # Header files & kalibrasi rasio 120 km/jam
│   ├── src/                                  # Implementasi FreeRTOS Task (Core 0 & Core 1)
│   └── platformio.ini                        # Konfigurasi build PlatformIO
├── telemetry-server/                         # Backend telemetri IoT (Node.js & WebSocket)
│   ├── package.json
│   └── server.js
├── dashboard-ui/                             # Instrument cluster digital (Skala 160 km/h)
│   ├── index.html
│   ├── styles.css
│   └── app.js
├── simulator/                                # Software-In-The-Loop (SITL) 120 km/h Simulator
│   ├── requirements.txt
│   └── vehicle_simulator.py
├── LICENSE                                   # Lisensi proyek (MIT)
└── README.md                                 # Dokumentasi utama proyek
```

---

## 🚀 Panduan Memulai Cepat (Quick Start)

### 1. Menjalankan Simulator Kendaraan 120 km/jam & HMI Dashboard
Anda dapat menguji respon akselerasi 120 km/jam dan sistem instrumen digital secara virtual:

```bash
# 1. Jalankan Backend Telemetri
cd telemetry-server
npm install
npm start

# 2. Jalankan Simulator Kendaraan 120 km/jam (Buka terminal baru)
cd ../simulator
pip install -r requirements.txt
python vehicle_simulator.py

# 3. Buka Dashboard HMI
# Buka file dashboard-ui/index.html di browser web Anda
```

---

## 📄 Lisensi

Proyek ini dilisensikan di bawah **MIT License**. Lihat file [LICENSE](LICENSE) untuk rincian lengkap.

Dikembangkan untuk kemajuan ekosistem kendaraan listrik performa tinggi Indonesia oleh **PT. Laksana Digital Industri**.
