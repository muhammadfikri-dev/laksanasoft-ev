# 🔌 Skematik & Wiring Diagram Lengkap (Laksanasoft-EV)

Dokumen ini menyajikan arsitektur pengkabelan terperinci, diagram skematik sirkuit tegangan tinggi (*High Voltage Traction*), sirkuit tegangan rendah (*Low Voltage Auxiliary*), serta alokasi pin I/O pada Vehicle Control Unit (VCU).

---

## 1. Skematik Sistem Daya Tegangan Tinggi (High Voltage 72V)

Sirkuit traksi tegangan tinggi dilengkapi dengan sistem pemutus darurat, proteksi arus lebih (Fuse), serta rangkaian *Pre-Charge* aktif untuk meredam arus kejut (*inrush current*) pengisian kapasitor inverter.

```
       +-------------------------------------------------------------+
       |               BATERAI PACK 72V (20S 45Ah)                   |
       |  [+] Positif (B+)                           [-] Negatif (B-) |
       +------+----------------------------------------------+-------+
              |                                              |
          [FUSE 150A]                                   [BMS B- Terminal]
              |                                              |
      [MANUAL SERVICE DISCONNECT]                            | (Internal FETs)
              |                                              |
              +-------------------+                          |
              |                   |                     [BMS P- Terminal]
        [MAIN CONTACTOR]   [PRE-CHARGE RELAY]                |
              |                   |                          |
              |            [RESISTOR 100R 50W]               |
              |                   |                          |
              +---------+---------+                          |
                        | (HV Positive Bus)                  | (HV Negative Bus)
                        v                                    v
       +-------------------------------------------------------------+
       |             INVERTER / MOTOR CONTROLLER (72V)               |
       |  [B+] Positif Input                         [B-] Negatif    |
       |                                                             |
       |  [U] Fasa U      [V] Fasa V      [W] Fasa W                 |
       +------+---------------+---------------+----------------------+
              |               |               |
              |   (Kabel Fasa Silikon 16mm²)  |
              v               v               v
       +-------------------------------------------------------------+
       |             MOTOR TRAKSI BLDC / PMSM (3000W)                |
       |  [U] Terminal    [V] Terminal    [W] Terminal               |
       |  [Hall U / V / W / +5V / GND / NTC Suhu]                    |
       +-------------------------------------------------------------+
```

### Prinsip Kerja Rangkaian Pre-Charge:
1. **Kondisi Awal (OFF):** Main Contactor dan Pre-charge Relay dalam kondisi terbuka (*Normally Open*).
2. **Fase Pre-Charge (1.5 Detik):** VCU mengaktifkan *Pre-Charge Relay*. Arus mengalir melalui Resistor 100 Ohm / 50W untuk mengisi kapasitor input inverter secara perlahan hingga tegangan kapasitor mencapai $\ge 95\%$ dari tegangan baterai ($V_{cap} \approx 70\text{V}$).
3. **Fase Aktif (Main Power ON):** VCU menyalakan *Main Contactor*. Kontak utama menghubungkan baterai langsung ke inverter tanpa percikan api (*zero spark*).
4. **Fase Selesai:** VCU menonaktifkan *Pre-Charge Relay* untuk menghemat energi.

---

## 2. Skematik Sistem Tegangan Rendah (Low Voltage 12V & 5V/3.3V)

```
[Baterai 72V B+] ---> [Fuse 10A] ---> [DC-DC Converter 72V -> 12V 20A] ---> [12V Bus]
                                                                                |
       +------------------------------------------------------------------------+
       |              |                |                  |                     |
       v              v                v                  v                     v
 [Kunci Kontak]  [Lampu Depan/   [Klakson /       [Buck Converter]       [Coil Contactor
  (Ignition SW)   Belakang LED]   Sein Relay]      12V -> 5V / 3.3V]      & Precharge]
       |                                                  |                     |
       v                                                  v                     v
 [VCU Wake-up Pin]                                 [VCU Main Power]       [Low-Side Driver]
```

---

## 3. Alokasi Pin I/O Vehicle Control Unit (ESP32-S3)

Berikut adalah *pinout mapping* terkonfigurasi pada modul ESP32-S3 VCU:

| Pin ESP32-S3 | Tipe Sinyal | Terhubung Ke | Fungsi & Deskripsi |
| :--- | :--- | :--- | :--- |
| **GPIO 1 (ADC1_CH0)** | Analog Input | Throttle APPS 1 | Sinyal analog gas primer ($0.8\text{V} - 4.2\text{V}$) |
| **GPIO 2 (ADC1_CH1)** | Analog Input | Throttle APPS 2 | Sinyal analog gas sekunder terbalik ($4.2\text{V} - 0.8\text{V}$) |
| **GPIO 4** | Digital Input (Pull-up) | Tuas Rem Kiri / Kanan | Input E-Braking / Interlock pengereman |
| **GPIO 5** | Digital Input (Pull-up) | Sensor Standar Samping | Interlock standar turun -> Motor Dinonaktifkan |
| **GPIO 6** | Digital Input (Pull-up) | Saklar Mode Berkendara | Mode Switch (ECO / URBAN / SPORT) |
| **GPIO 7** | Digital Input (Pull-up) | Saklar Reverse / Mundur| Tombol aktivasi gigi mundur kecepatan rendah (5 km/jam) |
| **GPIO 8** | Digital Output (MOSFET) | Coil Main Contactor | Mengaktifkan kontaktor utama 72V |
| **GPIO 9** | Digital Output (MOSFET) | Coil Pre-Charge Relay | Mengaktifkan relay pre-charge 1.5 detik |
| **GPIO 10** | Digital Output (PWM/DAC)| Inverter Throttle In | Mengirim sinyal gas terkondisi ke controller motor |
| **GPIO 11** | Digital Output | Inverter Brake / Regen | Sinyal aktivasi pengereman regeneratif ke inverter |
| **GPIO 12** | Digital Output | Inverter Reverse Pin | Sinyal arah putaran mundur ke inverter |
| **GPIO 17 (TXD)** | UART / CAN TX | SN65HVD230 CAN TX / BMS| Komunikasi paket CAN Bus / Serial BMS |
| **GPIO 18 (RXD)** | UART / CAN RX | SN65HVD230 CAN RX / BMS| Penerimaan paket CAN Bus / Serial BMS |
| **GPIO 43 (U0TXD)**| UART TX | Modul Seluler 4G LTE | Komunikasi AT-Command & data MQTT telemetri |
| **GPIO 44 (U0RXD)**| UART RX | Modul Seluler 4G LTE | Komunikasi respon seluler 4G LTE |
| **GPIO 38 (I2C SDA)**| I2C Data | Display Cluster HMI / OLED| Pengiriman data kecepatan, suhu, & SoC |
| **GPIO 39 (I2C SCL)**| I2C Clock | Display Cluster HMI / OLED| Clock sinkronisasi layar instrumen |

---

## 4. Diagram Pengkabelan Throttle APPS (Dual-Hall)

Untuk memenuhi standar keamanan *Fail-Safe Drive-by-Wire*:

```
          [+5V VCU VCC] ------------------------+------------+
                                                |            |
                                            [Hall 1]     [Hall 2]
                                                |            |
 [GPIO 1 (APPS 1 ADC)] <--- Filter RC <---------+            |
                                                             |
 [GPIO 2 (APPS 2 ADC)] <--- Filter RC <----------------------+
                                                |            |
          [GND VCU]     ------------------------+------------+
```

- **Sinyal APPS 1:** Naik secara proporsional dari $0.8\text{V}$ (posisi idle) hingga $4.2\text{V}$ (posisi bukaan penuh 100%).
- **Sinyal APPS 2:** Turun secara proporsional dari $4.2\text{V}$ (posisi idle) hingga $0.8\text{V}$ (posisi bukaan penuh 100%).
- **Persamaan Plausibilitas VCU:**
  $$V_{APPS1} + V_{APPS2} \approx 5.0\text{V} \pm 0.35\text{V}$$
  Jika terjadi deviasi melebihi $0.35\text{V}$ selama lebih dari $100\text{ ms}$, VCU langsung mengeksekusi *Emergency Throttle Cutoff*.
