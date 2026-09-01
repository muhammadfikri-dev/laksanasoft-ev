# 📘 Spesifikasi Sistem & Kalkulasi Rekayasa (Laksanasoft-EV)

Dokumen ini merinci spesifikasi teknis, kalkulasi performa mekanikal, kebutuhan daya elektrikal, dan parameter operasional untuk platform sepeda motor listrik Laksanasoft-EV.

---

## 1. Parameter Fisik & Mekanikal Kendaraan

| Parameter | Nilai / Satuan | Keterangan |
| :--- | :--- | :--- |
| **Tipe Rangka (Chassis)** | Tubular Steel Double Cradle (Konversi/Kustom) | Distribusi bobot optimal baterai di tengah (*mid-pack*) |
| **Berat Kosong (Curb Weight)** | 95 kg | Termasuk motor, inverter, dan rangka |
| **Berat Baterai (Battery Pack)** | 22 kg | 20S LiFePO4 / NMC dengan enclosure aluminium IP67 |
| **Gross Vehicle Weight (GVW)** | 210 kg | Asumsi bobot pengendara + beban: 93 kg |
| **Ukuran Ban Depan** | 90/90 - 14 inci | Rolling radius $r \approx 0.258\text{ m}$ |
| **Ukuran Ban Belakang** | 100/90 - 14 inci | Rolling radius $r \approx 0.267\text{ m}$ |
| **Koefisien Hambatan Udara ($C_d$)** | 0.65 | Profil aerodinamika sepeda motor komuter standar |
| **Frontal Area ($A$)** | 0.75 m² | Luas penampang depan kendaraan + pengendara |
| **Koefisien Gesek Gelinding ($C_{rr}$)** | 0.015 | Permukaan aspal rata |

---

## 2. Kalkulasi Kebutuhan Daya (Tractive Effort)

Kebutuhan daya total pada roda belakang ($P_{total}$) dihitung berdasarkan penjumlahan gaya hambat utama:

$$F_{total} = F_{rr} + F_{aero} + F_{grade} + F_{accel}$$

### A. Komponen Gaya Hambat:
1. **Rolling Resistance ($F_{rr}$):**
   $$F_{rr} = m \cdot g \cdot C_{rr} \cdot \cos(\theta)$$
   Untuk jalan datar ($\theta = 0^\circ$):
   $$F_{rr} = 210\text{ kg} \times 9.81\text{ m/s}^2 \times 0.015 = 30.9\text{ N}$$

2. **Aerodynamic Drag ($F_{aero}$):**
   $$F_{aero} = \frac{1}{2} \cdot \rho \cdot C_d \cdot A \cdot v^2$$
   Pada kecepatan jelajah $v = 60\text{ km/jam} = 16.67\text{ m/s}$ (densitas udara $\rho = 1.225\text{ kg/m}^3$):
   $$F_{aero} = 0.5 \times 1.225 \times 0.65 \times 0.75 \times (16.67)^2 = 82.9\text{ N}$$

3. **Daya pada Kecepatan Konstan 60 km/jam:**
   $$F_{cruise} = F_{rr} + F_{aero} = 30.9 + 82.9 = 113.8\text{ N}$$
   $$P_{wheel} = F_{cruise} \times v = 113.8\text{ N} \times 16.67\text{ m/s} = 1897\text{ Watt} \approx 1.9\text{ kW}$$

4. **Daya Akselerasi & Puncak (Tanjakan 15° & Akselerasi 0-60 km/jam dalam 6.5s):**
   - Dibutuhkan daya puncak (*Peak Power*) minimum $\ge 5.0\text{ kW}$ pada roda.
   - Efisiensi transmisi rantai/sabuk + motor: $\eta \approx 88\%$.
   - **Daya Motor yang Dipilih:** 3000W Nominal / 6000W Peak.

---

## 3. Spesifikasi Powertrain & Sistem Penggerak

### Motor Penggerak (Traction Motor)
- **Tipe:** Brushless DC (BLDC) / Permanent Magnet Synchronous Motor (PMSM) Mid-Drive
- **Tegangan Nominal:** 72V DC
- **Daya Nominal:** 3000 Watt (3 kW)
- **Daya Puncak (Peak Power):** 6000 Watt (6 kW @ 60 detik)
- **Torsi Nominal:** 14.5 Nm
- **Torsi Puncak (Peak Torque):** 45 Nm (pada shaft motor)
- **Max RPM:** 4800 RPM (Field Weakening hingga 5800 RPM)
- **Rasio Reduksi Gir/Rantai (Mid-Drive):** 1:4.2 (Sprocket Depan 14T, Sprocket Belakang 58T)
- **Torsi Roda Belakang Efektif:** $45\text{ Nm} \times 4.2 = 189\text{ Nm}$
- **Sensor:** Triple Hall Effect Sensors (120° phase angle) + NTC Thermistor 10k/100k internal.

### Inverter / Motor Controller
- **Tipe:** FOC (Field Oriented Control) Sinusoidal Wave Controller
- **Model Rekomendasi:** Votol EM150-2 / FarDriver ND72530 / Kelly KLS7230S
- **Rentang Tegangan Input:** 60V - 90V DC
- **Arus Baterai Maksimal (Bus Current):** 100A Continual / 150A Peak
- **Arus Fasa Maksimal (Phase Current):** 330A - 400A Peak
- **Metode Komunikasi:** CAN Bus 2.0B / Lin / UART
- **Proteksi Bawaan:** Over-voltage, Under-voltage, Over-temperature, Mosfet Short-circuit, Motor Lock-rotor protection.

---

## 4. Spesifikasi Sistem Penyimpanan Energi (Energy Storage System - ESS)

### Battery Pack Architecture
- **Konfigurasi Sel:** 20S 3P (atau 20S 1P prismatic)
- **Kimia Sel:** LiFePO4 (Lithium Iron Phosphate) atau NMC (Nickel Manganese Cobalt)
- **Tegangan Nominal Pack:** $20 \times 3.6V = 72.0\text{V}$ (NMC) atau $20 \times 3.2V = 64.0\text{V}$ (LiFePO4)
- **Tegangan Maksimal (Full Charge):** $20 \times 4.2V = 84.0\text{V}$ (NMC)
- **Tegangan Minimal (Cut-off):** $20 \times 3.0V = 60.0\text{V}$ (NMC)
- **Kapasitas Energi Total:** $72\text{V} \times 45\text{Ah} = 3.24\text{ kWh}$
- **Max Continuous Discharge:** $2\text{C} = 90\text{A}$
- **Max Peak Discharge (10s):** $3\text{C} = 135\text{A}$
- **Standard Charge Current:** $0.3\text{C} = 13.5\text{A}$

### Battery Management System (BMS)
- **Tipe:** Smart Programmable BMS dengan antarmuka UART/RS485/CAN Bus
- **Model Rekomendasi:** Daly Smart BMS 20S 150A / ANT BMS / JBD Smart BMS
- **Fitur Balancing:** Active Balancer 1.2A atau Passive Bleeding Balancing
- **Sensor Suhu:** 4x External NTC temperature probes (monitoring zona sel baterai).

---

## 5. Sistem Kontrol & Telemetri (VCU)

- **Mikrokontroler:** ESP32-S3-WROOM-1 (Xtensa dual-core 32-bit LX7, 240 MHz)
- **Sistem Operasi Real-Time:** FreeRTOS
- **Memori:** 512 KB SRAM + 8 MB Octal PSRAM + 16 MB SPI Flash
- **Antarmuka I/O:**
  - 2x ADC 12-bit terkalibrasi (APPS1 & APPS2 Throttle Input)
  - 1x CAN Bus Transceiver (SN65HVD230 / TJA1050)
  - 2x Hardware UART (BMS Communication & GPS NEO-6M/8M)
  - 4x Digital Optocoupler Inputs (Brake switch, Side stand, Mode Switch, Reverse)
  - 4x High-Side Mosfet/Relay Drivers (Main Contactor, Pre-Charge Relay, Headlight, Horn)
  - 1x I2C / SPI (OLED / TFT HMI Cluster Interface)
- **Konektivitas:** Wi-Fi 802.11 b/g/n, Bluetooth LE 5.0, Modul Eksternal SIMCOM A7670E (4G LTE Cat-1).
